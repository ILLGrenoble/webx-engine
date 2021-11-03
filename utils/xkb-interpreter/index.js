import { parseKeycodeFile, parseKeySymDefFile, parseSymbols } from './parser.js'
import { interpret } from './interpreter.js'
import { translate } from './translator.js';

const symbols = parseSymbols();
const keycodes = parseKeycodeFile();
const keysymdefs = parseKeySymDefFile();

const symbolMappings = interpret(symbols, keycodes, keysymdefs);

const translation = translate(symbolMappings);

console.log(translation);