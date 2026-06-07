import pandas as pd
import matplotlib.pyplot as plt


def load_latency_csv(path, percentile="P50"):
    df = pd.read_csv(path)

    grouped = (
        df.groupby("msg_count")[percentile]
        .agg(["mean", "std"])
        .reset_index()
    )

    return grouped


def plot_latency_multi_panel(
        output_dir,
        data_dict,
        percentile="P50"):

    payloads = list(data_dict.keys())

    fig, axes = plt.subplots(
        1,
        len(payloads),
        figsize=(18, 5),
        sharey=True
    )

    if len(payloads) == 1:
        axes = [axes]

    colors = {
        "AQUA": "blue",
        "eCAL": "green",
        "iceoryx2_nowait": "orange",
        "iceoryx2_wait": "red"
    }

    markers = {
        "AQUA": "o",
        "eCAL": "s",
        "iceoryx2_nowait": "^",
        "iceoryx2_wait": "D"
    }

    for ax, payload in zip(axes, payloads):

        systems = data_dict[payload]

        for system, df in systems.items():

            ax.plot(
                df["msg_count"],
                df["mean"],
                label=system,
                color=colors[system],
                marker=markers[system],
                linewidth=2,
                markersize=5
            )

            ax.fill_between(
                df["msg_count"],
                df["mean"] - df["std"],
                df["mean"] + df["std"],
                color=colors[system],
                alpha=0.15
            )

        ax.set_title(
            f"Payload Size: {payload}",
            fontsize=12
        )

        ax.set_xlabel(
            "Number of Messages",
            fontsize=12
        )

        ax.grid(
            True,
            linestyle=":",
            linewidth=0.7
        )

        ax.tick_params(
            axis='x',
            labelsize=11
        )

        ax.tick_params(
            axis='y',
            labelsize=11
        )

    axes[0].set_ylabel(
        f"{percentile} Latency (ns)",
        fontsize=12
    )

    handles, labels = axes[0].get_legend_handles_labels()

    fig.legend(
        handles,
        labels,
        loc="upper center",
        ncol=4,
        frameon=False,
        fontsize=12,
        bbox_to_anchor=(0.5, 1.03)
    )

    plt.tight_layout(
        rect=[0, 0, 1, 0.92]
    )

    plt.savefig(
        f"{output_dir}/{percentile.lower()}_latency_multi_panel.pdf"
    )

    plt.close()

if __name__ == "__main__":

    data_p50 = {
        "64KB": {
            "AQUA": load_latency_csv("USE_64K/aqua.csv", "P50"),
            "eCAL": load_latency_csv("USE_64K/ecal.csv", "P50"),
            "iceoryx2_nowait": load_latency_csv("USE_64K/iceoryx2_nowait.csv", "P50"),
            "iceoryx2_wait": load_latency_csv("USE_64K/iceoryx2.csv", "P50"),
        },
        "256KB": {
            "AQUA": load_latency_csv("USE_256K/aqua.csv", "P50"),
            "eCAL": load_latency_csv("USE_256K/ecal.csv", "P50"),
            "iceoryx2_nowait": load_latency_csv("USE_256K/iceoryx2_nowait.csv", "P50"),
            "iceoryx2_wait": load_latency_csv("USE_256K/iceoryx2.csv", "P50"),
        },
        "1MB": {
            "AQUA": load_latency_csv("USE_1M/aqua.csv", "P50"),
            "eCAL": load_latency_csv("USE_1M/ecal.csv", "P50"),
            "iceoryx2_nowait": load_latency_csv("USE_1M/iceoryx2_nowait.csv", "P50"),
            "iceoryx2_wait": load_latency_csv("USE_1M/iceoryx2.csv", "P50"),
        }
    }

    plot_latency_multi_panel(
        ".",
        data_p50,
        percentile="P50"
    )

    data_p90 = {
        "64KB": {
            "AQUA": load_latency_csv("USE_64K/aqua.csv", "P90"),
            "eCAL": load_latency_csv("USE_64K/ecal.csv", "P90"),
            "iceoryx2_nowait": load_latency_csv("USE_64K/iceoryx2_nowait.csv", "P90"),
            "iceoryx2_wait": load_latency_csv("USE_64K/iceoryx2.csv", "P90"),
        },
        "256KB": {
            "AQUA": load_latency_csv("USE_256K/aqua.csv", "P90"),
            "eCAL": load_latency_csv("USE_256K/ecal.csv", "P90"),
            "iceoryx2_nowait": load_latency_csv("USE_256K/iceoryx2_nowait.csv", "P90"),
            "iceoryx2_wait": load_latency_csv("USE_256K/iceoryx2.csv", "P90"),
        },
        "1MB": {
            "AQUA": load_latency_csv("USE_1M/aqua.csv", "P90"),
            "eCAL": load_latency_csv("USE_1M/ecal.csv", "P90"),
            "iceoryx2_nowait": load_latency_csv("USE_1M/iceoryx2_nowait.csv", "P90"),
            "iceoryx2_wait": load_latency_csv("USE_1M/iceoryx2.csv", "P90"),
        }
    }

    plot_latency_multi_panel(
    ".",
    data_p90,
    percentile="P90")

    data_p99 = {
        "64KB": {
            "AQUA": load_latency_csv("USE_64K/aqua.csv", "P99"),
            "eCAL": load_latency_csv("USE_64K/ecal.csv", "P99"),
            "iceoryx2_nowait": load_latency_csv("USE_64K/iceoryx2_nowait.csv", "P99"),
            "iceoryx2_wait": load_latency_csv("USE_64K/iceoryx2.csv", "P99"),
        },
        "256KB": {
            "AQUA": load_latency_csv("USE_256K/aqua.csv", "P99"),
            "eCAL": load_latency_csv("USE_256K/ecal.csv", "P99"),
            "iceoryx2_nowait": load_latency_csv("USE_256K/iceoryx2_nowait.csv", "P99"),
            "iceoryx2_wait": load_latency_csv("USE_256K/iceoryx2.csv", "P99"),
        },
        "1MB": {
            "AQUA": load_latency_csv("USE_1M/aqua.csv", "P99"),
            "eCAL": load_latency_csv("USE_1M/ecal.csv", "P99"),
            "iceoryx2_nowait": load_latency_csv("USE_1M/iceoryx2_nowait.csv", "P99"),
            "iceoryx2_wait": load_latency_csv("USE_1M/iceoryx2.csv", "P99"),
        }
    }

    plot_latency_multi_panel(
    ".",
    data_p99,
    percentile="P99"
)
