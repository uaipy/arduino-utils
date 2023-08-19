import { makeSerialPortReader } from "./infra/factory/serialPortReaderFactory.js";
import { handleSerialPort } from "./presentation/serialPortReader.js";
import { loggerInfoInstance } from "./infra/helpers/logger.js";

const serialPort = "/dev/ttyACM0";
const serialReader = makeSerialPortReader(serialPort);

serialReader.port.on("open", () => {
  loggerInfoInstance({ msg: `serial port open on: ${serialPort}` });
});

serialReader.parser.on("data", (data) => {
  handleSerialPort(data);
});
