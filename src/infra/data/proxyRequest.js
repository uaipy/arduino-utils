import axios from "axios";
import { loggerInfoInstance } from "../helpers/logger.js";
export function createProxyRequest(data) {
  const baseUrl = "http://localhost:3000";
  axios
    .post(`${baseUrl}/message/publish`, data, {
      headers: {
        "Content-type": "application/json; charset=UTF-8",
      },
    })
    .then(function (response) {
      loggerInfoInstance({
        msg: `data storage:`,
        statusCode: response.status,
        serverResponse: response.data,
      });
    })
    .catch(function (error) {
      throw error;
    });
}
