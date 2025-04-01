import matplotlib.pyplot as plt
import seaborn as sns
import os

CHARTS_DIR = 'charts'

def plot_device_params_time_series(device_params, path=None):
    fig, axs = plt.subplots(3, 1, figsize=(10, 15))
    fig.suptitle('Device Parameters')

    # Plot RSSI
    axs[0].plot(device_params['rssi'], label='RSSI', color='blue')
    axs[0].set_title('RSSI')
    axs[0].set_xlabel('Packet Number')
    axs[0].set_ylabel('RSSI (dBm)')
    axs[0].legend()

    # Plot SNR
    axs[1].plot(device_params['airtime'], label='AT', color='orange')
    axs[1].set_title('Airtime')
    axs[1].set_xlabel('Packet Number')
    axs[1].set_ylabel('Airtime (s)')
    axs[1].legend()

    # Plot Spreading Factor
    axs[2].plot(device_params['sf'], label='Spreading Factor', color='green')
    axs[2].set_title('Spreading Factor')
    axs[2].set_xlabel('Packet Number')
    axs[2].set_ylabel('Spreading Factor')
    axs[2].legend()

    plt.tight_layout()

    if path:
        plt.savefig(path)
    else:
        plt.show()


def plot_device_params_histograms(device_params, path=None):
    plt.figure(figsize=(12, 8))

    plt.subplot(2, 2, 1)
    sns.histplot(device_params['rssi'], bins=15, kde=True)
    plt.title('RSSI Distribution')
    plt.xlabel('RSSI (dBm)')
    plt.ylabel('Frequency')
    plt.grid()

    plt.subplot(2, 2, 2)
    sns.histplot(device_params['snr'], bins=15, kde=True)
    plt.title('SNR Distribution')
    plt.xlabel('SNR (dB)')
    plt.ylabel('Frequency')
    plt.grid()

    plt.subplot(2, 2, 3)
    sns.histplot(device_params['sf'], bins=15, kde=True)
    plt.title('Spreading Factor Distribution')
    plt.xlabel('Spreading Factor')
    plt.ylabel('Frequency')
    plt.grid()

    plt.subplot(2, 2, 4)
    sns.histplot(device_params['airtime'], bins=15, kde=True)
    plt.title('Airtime Distribution')
    plt.xlabel('Airtime (s)')
    plt.ylabel('Frequency')
    plt.grid()

    plt.tight_layout()

    if path:
        plt.savefig(path)
    else:
        plt.show()