import serial
import json
import time
import requests

SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
POST_URL = 'http://localhost:8000/readings'

def conectar_serial():
    while True:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print(f"[INFO] Conectado à porta {SERIAL_PORT}")
            return ser
        except serial.SerialException as e:
            print(f"[ERRO] Porta serial não disponível: {e}")
            print("[INFO] Tentando novamente em 5 segundos...")
            time.sleep(5)

def main():
    ser = None
    try:
        while True:
            ser = conectar_serial()
            while True:
                try:
                    linha = ser.readline().decode('utf-8', errors='ignore').strip()
                    if linha:
                        try:
                            dados = json.loads(linha)
                            print("[DADO RECEBIDO]", json.dumps(dados, indent=2))

                            # Enviando via POST
                            response = requests.post(POST_URL, json=dados)
                            if response.status_code == 200:
                                print("[ENVIO OK] Dados enviados com sucesso.")
                            else:
                                print(f"[ERRO ENVIO] Código {response.status_code}: {response.text}")

                        except json.JSONDecodeError:
                            print(f"[ERRO] JSON inválido recebido: {linha}")
                        except requests.RequestException as e:
                            print(f"[ERRO DE REDE] Falha ao enviar dados: {e}")
                    time.sleep(0.01)

                except serial.SerialException as e:
                    print(f"[ERRO SERIAL] Perda da conexão: {e}")
                    if ser and ser.is_open:
                        ser.close()
                    print("[INFO] Tentando reconectar à porta serial...")
                    break  # Sai do loop interno e volta a tentar conectar

    except KeyboardInterrupt:
        print("\n[INFO] Programa interrompido pelo usuário.")
    except Exception as e:
        print(f"[FALHA INESPERADA] {e}")
    finally:
        if ser and ser.is_open:
            ser.close()
            print("[INFO] Porta serial fechada.")

if __name__ == "__main__":
    main()
