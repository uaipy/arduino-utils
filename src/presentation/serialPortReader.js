import {
  loggerInfoInstance,
  loggerErrorInstance,
} from "../infra/helpers/logger.js";

const dataStorage = [];

export async function handleSerialPort(inputData) {
  try {
    const dataHandled = handleInputData(inputData);
    dataStorage.push(dataHandled);
    loggerInfoInstance({
      msg: `data storage:`,
      dataStorage,
    });
  } catch (err) {
    loggerErrorInstance({
      msg: `an error has occurred on send data to server`,
      err,
    });
  }
}

function handleInputData(data) {
  return JSON.parse(data.replace("\r", ""));
}
