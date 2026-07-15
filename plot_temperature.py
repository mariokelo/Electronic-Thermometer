import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates
from datetime import datetime
import sys

# ── Serial port ───────────────────────────────────────────────────────────────
PORT     = "COM4"   # Change this to your Arduino's COM port
BAUDRATE = 9600

# ── Data storage ──────────────────────────────────────────────────────────────
times  = []   # datetime objects
temps  = []   # °C readings
log_file = f"temperature_log_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

with open(log_file, 'w') as f:
    f.write("wall_clock,temperature_C\n")
print(f"Logging to: {log_file}")

# ── Serial connection ─────────────────────────────────────────────────────────
try:
    ser = serial.Serial(PORT, BAUDRATE, timeout=2)
    print(f"Connected to {PORT}")
except Exception as e:
    print(f"Could not open port: {e}")
    sys.exit(1)

# ── Plot setup ────────────────────────────────────────────────────────────────
fig, ax = plt.subplots(figsize=(12, 5))
fig.patch.set_facecolor('#1e1e2e')
ax.set_facecolor('#1e1e2e')
ax.tick_params(colors='white')
ax.xaxis.label.set_color('white')
ax.yaxis.label.set_color('white')
ax.title.set_color('white')
for spine in ax.spines.values():
    spine.set_edgecolor('#444466')

line,     = ax.plot([], [], color='#89dceb', linewidth=2, label='Temperature')
scatter   = ax.scatter([], [], color='#cba6f7', zorder=5, s=40)
temp_text = ax.text(0.02, 0.95, '', transform=ax.transAxes,
                    color='white', fontsize=12, va='top')

# Format x axis as HH:MM
ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
ax.xaxis.set_major_locator(mdates.AutoDateLocator())
fig.autofmt_xdate(rotation=45)

ax.set_xlabel('Time')
ax.set_ylabel('Temperature (°C)')
ax.set_title('Room Temperature — overnight log')
ax.legend(facecolor='#313244', labelcolor='white')
ax.grid(True, color='#313244', linestyle='--', alpha=0.5)

plt.tight_layout()

# ── Animation update ──────────────────────────────────────────────────────────
def update(_frame):
    while ser.in_waiting:
        try:
            raw = ser.readline().decode('utf-8', errors='ignore').strip()
        except Exception:
            continue

        if not raw or raw.startswith('#') or raw.startswith('millis') or raw.startswith('Commands'):
            continue

        parts = raw.split(',')
        if len(parts) != 3:
            continue

        try:
            temp = float(parts[2])
        except ValueError:
            continue

        now = datetime.now()
        times.append(now)
        temps.append(temp)

        with open(log_file, 'a') as f:
            f.write(f"{now.strftime('%H:%M:%S')},{temp:.2f}\n")

        print(f"[{now.strftime('%H:%M:%S')}] {temp:.2f} °C")

    if times:
        line.set_data(mdates.date2num(times), temps)
        scatter.set_offsets(list(zip(mdates.date2num(times), temps)))
        ax.relim()
        ax.autoscale_view()
        temp_text.set_text(f"Latest: {temps[-1]:.1f} °C")
        fig.autofmt_xdate(rotation=45)

    return line, scatter, temp_text

ani = animation.FuncAnimation(fig, update, interval=5000, cache_frame_data=False)
plt.show()

ser.close()