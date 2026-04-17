// window/createMainWindow.js
const path = require('path');
const url = require('url');

function createMainWindow({ app, BrowserWindow, isDev }) {
  const baseDir = app.isPackaged ? process.resourcesPath : path.join(__dirname, '..');

  const mainWindow = new BrowserWindow({
    width: 1280,
    height: 800,
    webPreferences: {
      preload: path.join(baseDir, 'preload.js'),
      additionalArguments: [`--isDev=${isDev}`],
      contextIsolation: false,
      nodeIntegration: true,
    },
  });

  mainWindow.loadURL(
    url.format({
      pathname: path.join(baseDir, 'index.html'),
      protocol: 'file:',
      slashes: true,
    })
  );

  return mainWindow;
}

module.exports = { createMainWindow };
