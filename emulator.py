import os
import pty
import time

def run_emulator():
    # Создаем виртуальный порт
    master, slave = pty.openpty()
    port_name = os.ttyname(slave)
    print(f"[*] Эмулятор запущен. ПОРТ: {port_name}")
    print(f"[*] Скопируй этот путь в GUI и нажми ЗАПУСТИТЬ")

    # Начальные параметры
    start_time = time.time()
    current_ma = 250.0 # Базовый ток

    try:
        while True:
            # 1. Читаем входящие команды от GUI (например, vout 3800)
            try:
                # Читаем без блокировки, если есть данные
                import select
                r, w, e = select.select([master], [], [], 0)
                if master in r:
                    data = os.read(master, 1024).decode().strip()
                    if data:
                        print(f"[RX FROM GUI]: {data}")
            except Exception as e:
                pass

            # 2. Генерируем телеметрию в ТЕКСТОВОМ формате
            elapsed = int(time.time() - start_time)

            # Формат: каждая строка начинается с тега, как ждет C++
            # [SEC] - секунды, [VOL] - мВ, [CUR] - мкА
            telemetry = (
                f"[SEC] {elapsed}\n"
                f"[VOL] 3700\n"
                f"[CUR] {int(current_ma * 1000)}\n"
            )

            os.write(master, telemetry.encode())
            print(f"[TX TO GUI]: Time={elapsed}s, Current={current_ma}mA")

            # Небольшая вариация тока для "живого" графика
            import random
            current_ma += random.uniform(-10, 10)
            current_ma = max(100, min(1000, current_ma))

            time.sleep(1) # Посылаем раз в секунду

    except KeyboardInterrupt:
        print("\n[*] Остановка эмулятора...")
    finally:
        os.close(master)
        os.close(slave)

if __name__ == "__main__":
    run_emulator()
