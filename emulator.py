import os
import pty
import tty
import time
import select
import fcntl
import random

def run_emulator():
    master, slave = pty.openpty()

    # PTY must be in raw mode so QSerialPort gets unprocessed bytes.
    # Without this, the line discipline buffers/mangles data and readyRead never fires.
    tty.setraw(slave)

    # Non-blocking writes to master so we don't stall if GUI isn't reading
    flags = fcntl.fcntl(master, fcntl.F_GETFL)
    fcntl.fcntl(master, fcntl.F_SETFL, flags | os.O_NONBLOCK)

    port_name = os.ttyname(slave)
    print(f"[*] Эмулятор запущен. ПОРТ: {port_name}")
    print(f"[*] Вставь путь в GUI и нажми СТАРТ")

    start_time = time.time()
    current_ma = 250.0
    BASE_VOLTAGE_MV = 3700

    try:
        while True:
            # Read commands from GUI (non-blocking)
            r, _, _ = select.select([master], [], [], 0)
            if r:
                try:
                    data = os.read(master, 1024).decode(errors='ignore').strip()
                    if data:
                        print(f"[RX FROM GUI]: {data}")
                except OSError:
                    pass

            elapsed = int(time.time() - start_time)

            # Simulate gradual discharge: voltage drops ~1mV per second
            voltage_mv = max(2800, BASE_VOLTAGE_MV - elapsed)

            telemetry = (
                f"[SEC] {elapsed}\n"
                f"[VOL] {voltage_mv}\n"
                f"[CUR] {int(current_ma * 1000)}\n"
            )

            try:
                os.write(master, telemetry.encode())
                print(f"[TX TO GUI]: Time={elapsed}s, V={voltage_mv}mV, Current={current_ma:.2f}mA")
            except BlockingIOError:
                print("[WARN] PTY buffer full, skipping frame")

            current_ma += random.uniform(-10, 10)
            current_ma = max(100, min(500, current_ma))

            time.sleep(1)

    except KeyboardInterrupt:
        print("\n[*] Остановка эмулятора...")
    finally:
        os.close(master)
        os.close(slave)

if __name__ == "__main__":
    run_emulator()
