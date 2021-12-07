import matplotlib.pyplot as plt
import pandas as pd

if __name__ == "__main__":
    df = pd.read_csv("sample_d.csv")
    df_columns = df.keys()

    df.plot(x=df_columns[0], y=df_columns[1:], kind="line", marker="o")

    plt.xticks(rotation=25)
    plt.xlabel("Number of nodes")
    plt.ylabel("Time Taken")
    plt.title(
        "Dijkstra's Algortihm - Time taken for different number of threads", fontsize=20
    )
    plt.grid()
    plt.legend()
    plt.show()
