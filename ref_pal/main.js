const { ipcMain, app, BrowserWindow } = require('electron');

// ---- window ----
const { createMainWindow } = require('./window/createMainWindow');

// ---- viss ----
const { startViss } = require('./viss');

let mainWindow;
const isDev = !app.isPackaged;

async function onAppReady() {
  mainWindow = createMainWindow({ app, BrowserWindow, isDev });

  // Start subscribe
  startViss({ mainWindow: mainWindow });
}

app.whenReady().then(() => {
  onAppReady().catch((error) => {
    console.log('[main] Unexpected error in onAppReady:', error.message || error);
  });
});

ipcMain.on('client-event', (_event, msg) => {
  console.log('PUSH from renderer:', msg);
});
