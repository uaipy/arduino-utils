import serial
import json
import time

SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200

ser = None 
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"[INFO] Lendo da porta {SERIAL_PORT}...")
    while True:
        linha = ser.readline().decode('utf-8', errors='ignore').strip() 
        if linha:
            try:
                dados = json.loads(linha)
                print("[DADO RECEBIDO]", json.dumps(dados, indent=2))
            except json.JSONDecodeError:
                print(f"[ERRO] JSON inválido recebido: {linha}")
        
        time.sleep(0.01)

except serial.SerialException as e:
    print(f"[FALHA] Não foi possível abrir a porta serial: {e}")
except KeyboardInterrupt:
    print("\n[INFO] Programa interrompido pelo usuário.")
except Exception as e:
    print(f"[FALHA INESPERADA] {e}")
finally:
    if ser and ser.is_open:
        ser.close()
        print("[INFO] Porta serial fechada.")