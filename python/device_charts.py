import matplotlib.pyplot as plt
import seaborn as sns
import os


def save_fig(plt, file_name, dir):
    if not os.path.exists(dir):
        os.makedirs(dir)
        print(f"Directory {dir} created.")
    else:
        print(f"Directory {dir} already exists.")
    path = os.path.join(dir, file_name)
    plt.savefig(path)
    print(f"Saved chart to {path}")


def plot_device_params_time_series(device_params, device_id, dir=None):
    """
    Plot time series of device parameters.
    1. RSSI
    2. Airtime
    3. Spreading Factor
    Parameters:
    device_params (dict): Dictionary containing device parameters.
        {
            'rssi': [rssi_values],
            'snr': [snr_values],
            'sf': [sf_values],
            'airtime': [airtime_values]
        }
        device_id (str): Device ID.
        dir (str): Directory to save the plots. If None, show the plots.
    """
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

    plt.title('Time series ID=' + device_id)
    plt.tight_layout()

    if dir:
        save_fig(plt, f'{device_id}_time_series.png', dir)
    else:
        plt.show()


def plot_device_params_histograms(device_params, device_id, dir=None):
    """
    Plot histograms of device parameters.
    Parameters:
    device_params (dict): Dictionary containing device parameters.
        {
            'rssi': [rssi_values],
            'snr': [snr_values],
            'sf': [sf_values],
            'airtime': [airtime_values]
        }
        device_id (str): Device ID.
        dir (str): Directory to save the plots. If None, show the plots.
    """
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

    plt.title('Parameters Histograms ID=' + device_id)
    plt.tight_layout()

    if dir:
        save_fig(plt, f'{device_id}_histograms.png', dir)
    else:
        plt.show()