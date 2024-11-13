import json
import os
import platform
import re
import subprocess
import time

from benchtool.BenchTool import BenchTool, Entry
from benchtool.Types import Config, LogLevel, ReplaceLevel, TrialArgs


class Cn(BenchTool):
    def __init__(self,  results: str, log_level: LogLevel = LogLevel.INFO, replace_level: ReplaceLevel = ReplaceLevel.REPLACE):
        super().__init__(
            Config(start="//",
                   end="//",
                   ext=".c",
                   path="workloads/Cn",
                   ignore="test",
                   strategies='',
                   impl_path='./',
                   spec_path=''),
            results, log_level, replace_level)

    def all_properties(self, workload: Entry) -> list[str]:
        spec = os.path.join(workload.path, self._config.spec_path)
        with open(spec) as f:
            contents = f.readlines()
            return contents

    def _build(self, workload_path: str):
        pass

    def _run_trial(self, workload_path: str, args: TrialArgs):
        with self._change_dir(workload_path):
            cmd = ['cn', 'test', '--output-dir=test/', '--until-timeout=60', '--exit-fast', 'src.c', '--only', args.property]
            if args.strategy.startswith('no_'):
                cmd.extend(['--disable', args.strategy[3:]]) # cuts off "no_" from strategy name
            if args.strategy == 'nothing':
                cmd.extend(['--disable', 'reorder,picks,consistency,lift_constraints'])

            results = []
            self._log(
                f"Running {args.workload},{args.strategy},{args.mutant},{args.property}", LogLevel.INFO)
            for _ in range(args.trials):
                try:
                    trial_result = {
                        "workload": args.workload,
                        "discards": None,
                        "foundbug": None,
                        "strategy": args.strategy,
                        "mutant": args.mutant,
                        "passed": None,
                        "property": args.property,
                        "time": None
                    }
                    begin_time = time.perf_counter()
                    process = subprocess.Popen(
                        cmd,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True
                    )
                    stdout_data, stderr_data = process.communicate(
                        timeout=args.timeout)
                    end_time = time.perf_counter()
                    try:
                        res = stdout_data.splitlines()[-1]
                        m1 = re.match(r'cases: \d+, passed: \d+, failed: (?P<failed>\d+), errored: \d+', res)
                    except IndexError:
                        m1 = None
                    if m1 is None:
                        self._log(
                            f"Unexpected! Error Processing {args.strategy} Output:",
                            LogLevel.ERROR)
                        self._log(f"[{stdout_data}]", LogLevel.ERROR)
                        self._log(f"[{stderr_data}]", LogLevel.ERROR)
                        trial_result["foundbug"] = False
                        trial_result["discards"] = 0
                        trial_result["passed"] = 0
                        trial_result["time"] = -1
                    else:
                        m2s = re.findall(r'Testing \S+:( (?P<passes>\d+) runs)?(; (?P<discards>\d+) discarded)?(\r|\n)(?!T)', stdout_data)

                        passed = 0
                        discards = 0
                        for m2 in m2s:
                            passed += int(m2[1] or '0')
                            discards += int(m2[3] or '0')

                        self._log(f"{args.strategy} Result: {res}", LogLevel.INFO)
                        trial_result["foundbug"] = int(m1.group('failed')) > 0
                        trial_result["discards"] = discards
                        trial_result["passed"] = passed
                        trial_result["time"] = end_time - begin_time

                except subprocess.TimeoutExpired as e:
                    print(f"Process Timed Out {process.pid}")
                    process.kill()
                    os.system("pkill tests.out")

                    m2s = re.findall(r'Testing \S+:( (?P<passes>\d+) runs)?(; (?P<discards>\d+) discarded)?((\r|\n)(?!T)|$)', e.stdout.decode("utf-8"))

                    passed = 0
                    discards = 0
                    for m2 in m2s:
                        passed += int(m2[1] or '0')
                        discards += int(m2[3] or '0')

                    trial_result["foundbug"] = False
                    trial_result["discards"] = discards
                    trial_result["passed"] = passed
                    trial_result["time"] = args.timeout
                    self._log(
                        f"{args.strategy} Result: Timeout", LogLevel.INFO)

                except subprocess.CalledProcessError as e:
                    self._log(f"Error Running {args.strategy}:", LogLevel.ERROR)
                    self._log(f"[{e.stdout}]", LogLevel.ERROR)
                    self._log(f"[{e.stderr}]", LogLevel.ERROR)
                    trial_result["foundbug"] = False
                    trial_result["discards"] = 0
                    trial_result["passed"] = 0
                    trial_result["time"] = -1

                results.append(trial_result)
                if args.short_circuit and trial_result['time'] == args.timeout:
                    break
                elif trial_result['time'] == -1:
                    self._log(f"Exiting due to erroneous trial",
                              LogLevel.ERROR)
                    exit(0)

            try:
                json.dump(results, open(args.file, 'w'))
            except TypeError as e:
                print(results)
                raise e

    def _preprocess(self, workload: Entry) -> None:
        pass