import pino from "pino"

const logger = pino({});

export function loggerInfoInstance(data) {
  logger.info(data);
}

export function loggerErrorInstance(data) {
  logger.error(data);
}

export function loggerDebugInstance(data) {
  logger.debug(data);
}