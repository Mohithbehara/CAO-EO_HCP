import os
import psutil
import time

# Defining path to sysfs interface for CPU frequency scaling
cpu_freq_path = "/sys/devices/system/cpu/cpu0/cpufreq/"

def set_cpu_frequency(freq_kHz):
    # Check if  governor is set to "userspace" (may require root access)
    governor_file = os.path.join(cpu_freq_path, "scaling_governor")
    with open(governor_file, "r") as f:
        governor = f.read().strip()
        if governor != "userspace":
            print("Changing scaling governor to 'userspace' (requires root access)...")
            with open(governor_file, "w") as gf:
                gf.write("userspace")

    # Set CPU frequency (requires root access)
    freq_file = os.path.join(cpu_freq_path, "scaling_setspeed")
    with open(freq_file, "w") as f:
        f.write(str(freq_kHz))

# Defining thresholds for CPU utilization and I/O activity
idle_threshold = 10  # CPU utilization threshold for idle time
io_threshold = 5     # I/O activity threshold

while True:
    # Get the current CPU utilization as a percentage
    cpu_utilization = psutil.cpu_percent(interval=1)

    # Get the current I/O activity as a percentage
    io_activity = psutil.disk_io_counters().percent

    if cpu_utilization < idle_threshold or io_activity <= io_threshold:
        # If both CPU is idle and I/O activity is low, set a lower frequency (e.g. 1 GHz)
        set_cpu_frequency(1000000)
    else:
        # Otherwise, set a higher frequency (e.g. 2 GHz)
        set_cpu_frequency(2000000)

    print(f"CPU utilization: {cpu_utilization}%, I/O activity: {io_activity}%")
    time.sleep(5)  # Adjust the sleep interval
