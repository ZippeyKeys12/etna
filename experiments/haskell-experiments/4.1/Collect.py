import argparse
import os

from benchtool.Haskell import Haskell
from benchtool.Types import ReplaceLevel, TrialConfig
from benchtool.Tasks import tasks

# Section 4.1 (Comparing Frameworks)

def collect(results: str, optimize: bool = True):
    tool = Haskell(results, replace_level=ReplaceLevel.SKIP)

    for workload in tool.all_workloads():
        if workload.name not in ['BST', 'RBT', 'STLC', 'FSUB']:
            continue

        for variant in tool.all_variants(workload):
            if variant.name == 'base':
                continue

            run_trial = None

            for strategy in tool.all_strategies(workload):
                if strategy.name not in ['Correct', 'Quick', 'Lean', 'Small']:
                    continue

                for property in tool.all_properties(workload):
                    if workload.name in ['BST', 'RBT']:
                        if property.split('_')[1] not in tasks[workload.name][variant.name]:
                            continue

                    if optimize:
                        # TO SAVE TIME:
                        # Run only 1 trial for deterministic strategies
                        trials = 1 if strategy.name in ['Lean', 'Small'] else 10

                        # Also, stop trials as soon as fail to find bug.
                        # (via short_circuit flag below)
                    else:
                        trials = 10

                    # Don't compile tasks that are already completed.
                    completed = f'{results}/completed.txt'
                    if os.path.isfile(completed):
                        with open(completed) as f:
                            file = f'{workload.name},{strategy.name},{variant.name},{property}'
                            if file in f.read():
                                continue

                    if not run_trial:
                        run_trial = tool.apply_variant(workload, variant)


                    cfg = TrialConfig(workload=workload,
                                      strategy=strategy.name,
                                      property=property,
                                      trials=trials,
                                      timeout=65,
                                      short_circuit=optimize)

                    run_trial(cfg)

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument('--data', help='path to folder for JSON data')
    p.add_argument('--full',
                   help='flag to disable faster version of experiment',
                   action='store_false')
    args = p.parse_args()

    results_path = f'{os.getcwd()}/{args.data}'
    collect(results_path, args.full)
