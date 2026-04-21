import os
import pty
import time
import fcntl
import termios

def run_emulator():
    # 1. Создаем виртуальную пару портов (Master/Slave)
    master, slave = pty.openpty()
    s_name = os.ttyname(slave)

    # 2. Выключаем ECHO (чтобы устройство не читало свои же отправленные данные)
    attr = termios.tcgetattr(master)
    attr[3] = attr[3] & ~termios.ECHO
    termios.tcsetattr(master, termios.TCSANOW, attr)

    # 3. Делаем чтение и запись неблокирующими (чтобы скрипт не зависал при переполнении буфера)
    fl = fcntl.fcntl(master, fcntl.F_GETFL)
    fcntl.fcntl(master, fcntl.F_SETFL, fl | os.O_NONBLOCK)

    # 4. Определяем пути к логам на рабочем столе
    out_log_path = os.path.expanduser("~/Desktop/TO_QT_telemetry.txt")
    in_log_path = os.path.expanduser("~/Desktop/FROM_QT_commands.txt")

    print(f"✅ Эмулятор запущен!")
    print(f"👉 Подключи Qt к порту: {s_name}")
    print(f"📂 Лог ТЕЛЕМЕТРИИ (OUT) -> {out_log_path}")
    print(f"📂 Лог КОМАНД (IN)     -> {in_log_path}")
    print("-----------------------------------------")

    # 5. Открываем файлы и запускаем цикл
    with open(out_log_path, "a") as out_f, open(in_log_path, "a") as in_f:
        out_f.write(f"\n--- Новая сессия: {time.ctime()} ---\n")
        in_f.write(f"\n--- Новая сессия: {time.ctime()} ---\n")

        try:
            while True:
                # --- ШАГ 1: ОТПРАВКА ДАННЫХ В QT (Имитация датчиков) ---
                timestamp = int(time.time() * 1000)
                # Имитируем: Напряжение 12.5V, Ток 500mA
                telemetry = f"T:{timestamp} V:12500 I:500\n"

                try:
                    os.write(master, telemetry.encode())
                    out_f.write(telemetry)
                    out_f.flush()
                except BlockingIOError:
                    # Если Qt не успевает читать, буфер переполняется. Просто пропускаем пакет.
                    pass

                # --- ШАГ 2: ПРИЕМ КОМАНД ИЗ QT (Результат работы модели) ---
                try:
                    data = os.read(master, 1024)
                    if data:
                        # Декодируем и очищаем строку от лишних символов
                        line = data.decode().strip()
                        if line:
                            in_f.write(f"{line}\n")
                            in_f.flush()
                            print(f"📥 Команда от Qt: {line}")
                except (BlockingIOError, OSError):
                    # Данных в порту пока нет, идем дальше
                    pass

                # Интервал 100мс
                time.sleep(0.1)

        except KeyboardInterrupt:
            print("\n🛑 Эмулятор остановлен пользователем.")

if __name__ == "__main__":
    run_emulator()
