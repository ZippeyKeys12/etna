import argparse
from benchtool.Analysis import *
from benchtool.Plot import *
from functools import partial


def analyze(results: str, images: str):
    df = parse_results(results)

    if not os.path.exists(images):
        os.makedirs(images)

    # Generate task bucket charts used in Figure 3.
    for workload in ['BST', 'DLL', 'ImperativeQueue', 'Runway', 'SortedList', 'RingQueue', 'SLL']:
        times = partial(stacked_barchart_times, case=workload, df=df)
        times(
            strategies=[
                'default',
                'no_reorder',
                'no_picks',
                # 'no_flatten',
                'no_consistency',
                'no_lift_constraints',
                'nothing'
            ],
            colors=['#000000', '#900D0D', '#DC5F00', '#6D0E56', '#243763', '#D61C4E'],
            limits=[0.1, 1, 10, 60],
            limit_type='time',
            image_path=images,
            show=False,
        )

    # Compute solve rates.
    dfa = overall_solved(df, 'all').reset_index()
    dfa = dfa.groupby('strategy').sum(numeric_only=True)
    dfa['percent'] = dfa['solved'] / dfa['total']
    print(dfa)


if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument('--data', help='path to folder for JSON data')
    p.add_argument('--figures', help='path to folder for figures')
    args = p.parse_args()

    results_path = f'{os.getcwd()}/{args.data}'
    images_path = f'{os.getcwd()}/{args.figures}'
    analyze(results_path, images_path)
