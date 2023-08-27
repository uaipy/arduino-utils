import { loggerErrorInstance } from "../infra/helpers/logger.js";
import { createProxyRequest } from "../infra/data/proxyRequest.js";

export async function handleSerialPort(inputData) {
  try {
    const dataHandled = handleInputData(inputData);
    const dataParsed = JSON.parse(dataHandled);
    const dataForward = {
      ...dataParsed,
      messageReadDate: new Date(),
    };
    createProxyRequest(JSON.stringify(dataForward));
  } catch (err) {
    loggerErrorInstance({
      msg: `an error has occurred on send data to server`,
      err,
    });
  }
}

function handleInputData(data) {
  return data.replace("\r", "");
}
