# Estação de Controle de Irrigação com ESP32 LoRa

Este projeto demonstra uma estação de controle de irrigação utilizando as placas **Heltec Automation WiFi LoRa 32 V3.1** (uma como emissora e outra como receptora). Ele coleta dados de sensores (temperatura, umidade e chuva) e os envia sem fio via LoRa. Além disso, um módulo RTC é usado para manter o tempo com precisão.

---

## Funcionalidades

- **Comunicação LoRa**: Transmissão sem fio utilizando as placas Heltec WiFi LoRa 32 V3.1.
- **Integração de Sensores**:
  - **DHT11** – mede a temperatura e umidade do solo.
  - **SHT20** – mede a temperatura e umidade do ar.
  - **Pluviômetro** – baseado em sensor Hall (modelo 44E) que contabiliza as basculadas do pluviômetro.
- **Relógio em Tempo Real (RTC)**: Utiliza módulo DS3231 para manter a hora com precisão.
- **Formato JSON**: Os dados dos sensores são formatados em JSON antes da transmissão.
- **Receptor Simples**: Recebe os pacotes LoRa e imprime o conteúdo JSON com timestamp.

---

## Hardware Utilizado

- 2x placas **Heltec Automation WiFi LoRa 32 V3.1** (uma configurada como emissora, outra como receptora)
- Sensor **DHT11** (para temperatura e umidade do solo)
- Sensor **SHT20** (para temperatura e umidade do ar)
- 2x sensores Hall **44E**, um em cada lado da bascula, para detectar e contabilizar as basculadas do pluviômetro.
- Módulo RTC **DS3231**

---

## Configuração do Módulo RTC

O código do RTC (`rtc_config.ino`) precisa ser executado apenas uma vez para inicializar e definir a hora atual no módulo DS3231, usando o tempo de compilação do sketch. Depois disso, o módulo RTC mantém a contagem do tempo de forma independente, mesmo com o sistema desligado.

---

## Estrutura do Projeto

- `lora_sender.ino`: Lê os sensores, obtém a data e hora do módulo RTC, formata os dados em JSON e transmite via LoRa.
- `lora_receiver.ino`: Recebe os pacotes LoRa e imprime os dados JSON.
- `rtc_config.ino`: Inicializa e define a hora do módulo RTC (executar apenas uma vez no emissor).

---

## Formato dos Dados JSON

Todos os dados transmitidos e recebidos seguem esta estrutura:

```json
{
  "dht_temperature": 23,
  "dht_humidity": 80,
  "sht_temperature": 23,
  "sht_humidity": 80,
  "rain": 10,
  "local_date_read": "YYYY-MM-DDTHH:mm:ss.fffZ",
  "device_id": 1
}
