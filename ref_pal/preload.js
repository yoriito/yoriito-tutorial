
const path = require('path');
const projectName = 'widget-3d-car-unity';

const isDev = process.argv.includes('--isDev=true');
const buildDir = isDev
  ? path.join(__dirname, 'Build')
  : path.join(process.resourcesPath, 'Build');

window.unityConfig = {
  buildUrl: buildDir,
  loaderUrl: path.join(buildDir, `${projectName}.loader.js`)
};

