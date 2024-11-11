import argparse
import os

from benchtool.Cn import Cn
from benchtool.Types import TrialConfig
from benchtool.Tasks import tasks

# Section 4.1 (Comparing Frameworks)


def collect(results: str):
    tool = Cn(results)

    for workload in tool.all_workloads():
        if workload.name not in ['BST', 'DLL', 'ImperativeQueue', 'Runway','SortedList']:
            continue

        for variant in tool.all_variants(workload):
            if variant.name == 'base':
                # Don't run on base (non-buggy) implementation.
                continue

            run_trial = None

            for strategy in tool.all_strategies(workload):
                if strategy.name not in ['default']:
                    continue

                # TO SAVE TIME:
                # Run only 1 trial for deterministic strategies
                trials = 10

                # Also, stop trials as soon as fail to find bug.
                # (via short_circuit flag below)

                # See README discussion about LeanCheck.
                timeout = 65

                # Don't compile tasks that are already completed.
                finished = set(os.listdir(results))
                file = f'{workload.name},{strategy.name},{variant.name},{workload.name}'
                if f'{file}.json' in finished:
                    continue

                if not run_trial:
                    run_trial = tool.apply_variant(workload, variant)

                cfg = TrialConfig(workload=workload,
                                  strategy=strategy.name,
                                  property=workload.name,
                                  trials=trials,
                                  timeout=timeout,
                                  short_circuit=True)

                run_trial(cfg)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--data', help='path to folder for JSON data')
    args = p.parse_args()

    results_path = f'{os.getcwd()}/{args.data}'
    collect(results_path)
