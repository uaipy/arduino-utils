import { SerialPort } from "serialport";
import { ReadlineParser } from "serialport";

export function makeSerialPortReader(serialPort) {
  const port = new SerialPort({
    path: serialPort,
    baudRate: 9600,
  });
  const parser = port.pipe(new ReadlineParser({ delimitador: "\n" }));
  return {
    port,
    parser,
  };
}
