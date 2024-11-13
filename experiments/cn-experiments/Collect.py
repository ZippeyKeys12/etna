import argparse
import os

from benchtool.Cn import Cn
from benchtool.Types import TrialConfig
from Tasks import tasks

# Section 4.1 (Comparing Frameworks)


def collect(results: str):
    tool = Cn(results)

    for workload in tool.all_workloads():
        if workload.name not in [
            'BST',
            'DLL',
            'ImperativeQueue',
            'Runway',
            'SortedList',
            'RingQueue',
            'SLL'
        ]:
            continue

        for variant in tool.all_variants(workload):
            if variant.name == 'base':
                # Don't run on base (non-buggy) implementation.
                continue

            run_trial = None

            for strategy in [
                    'default',
                    'no_reorder',
                    # 'no_picks',
                    # 'no_flatten',
                    'no_consistency',
                    'no_lift_constraints',
                    'nothing'
            ]:
                for property in tasks[workload.name][variant.name]:
                    trials = 25

                    # Also, stop trials as soon as fail to find bug.
                    # (via short_circuit flag below)

                    # See README discussion about LeanCheck.
                    timeout = 65

                    # Don't compile tasks that are already completed.
                    finished = set(os.listdir(results))
                    file = f'{workload.name},{strategy},{variant.name},{property}'
                    if f'{file}.json' in finished:
                        continue

                    if not run_trial:
                        run_trial = tool.apply_variant(workload, variant)

                    cfg = TrialConfig(workload=workload,
                                      strategy=strategy,
                                      property=property,
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
