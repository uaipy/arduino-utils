import axios from "axios";

export function createProxyRequest(data) {
  axios
    .post("/device", data, {
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
