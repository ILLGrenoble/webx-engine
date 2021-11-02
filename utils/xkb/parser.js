import * as fs from 'fs';

const xkbSymbols = [{
    file: 'fr',
    inherits: 'latin'
}, {
    file: 'gb',
    inherits: 'latin'
}, {
    file: 'us',
    inherits: null
}];


const parseSymbolFile = (file) => {
    const data = fs.readFileSync(`symbols/${file}`, 'utf8');
    return data.split('\n')
        .filter(s => s.length > 0)
        .map(s => s.replace(/\s/g, ''))
        .map(line => {
            const pattern = 'key<(.*)>{\\[(.*)]};';
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        }).filter(matches => matches != null)
        .map(matches => {
            const scancode = matches[1];
            const keysyms = matches[2].trim().split(',').map(v => v.trim());
            return { scancode, keysyms };
        });
}


export const parseSymbols = () => {
    const parsed = xkbSymbols.map(xkbSymbol => {
        const { file, inherits } = xkbSymbol;
        const symbols = parseSymbolFile(file);
    
        if (inherits) {
            parseSymbolFile(inherits).forEach(key => {
                if (!symbols.find(f => f.keycode === key.keycode)) {
                    symbols.push(key);      
                }
            });
        }
    
        return { layout: file, symbols }
    });

    return parsed;

}

export const parseKeycodeFile = () => {
    const data = fs.readFileSync(`keycodes/evdev`, 'utf8');
    const parsed = data.split('\n')
        .filter(s => s.length > 0)
        .map(s => s.replace(/\s/g, ''))
        .map(line => {
            const pattern = '(.*?)<(.*)>=(.*);.*';
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        }).filter(matches => matches != null)
        .map(matches => {
            const isAlias = matches[1] === 'alias';
            const scancode = matches[2];
            const alias = isAlias ? matches[3].substr(1, matches[3].length - 2) : null;
            const keycode =!isAlias ? parseInt(matches[3]) : null;
            return { scancode, keycode, alias };
        });

    // convert aliases
    return parsed.reduce((reduced, value) => {
        const { scancode, keycode, alias } = value;
        if (value.alias === null) {
            reduced.push({scancode, keycode})
        } else {
            const aliased = parsed.find((entry) => entry.scancode === alias);
            if (aliased) {
                reduced.push({scancode, keycode: aliased.keycode});
            } else {
                console.error(`couldn't find alias for scancode ${scancode}`);
            }
        }
        return reduced;
    }, []);
}

export const parseKeySymDefFile = () => {
    const data = fs.readFileSync(`symbols/keysymdef.h`, 'utf8');
    return data.split('\n')
        .filter(s => s.length > 0)
        .map(line => {
            const pattern = '#define *XK_(.*)(0x[0-9a-zA-Z]*)';
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        })
        .filter(matches => matches != null)
        .map(matches => {
            const name = matches[1].trim();
            const value = parseInt(matches[2]);
            return { name, value };
        });
}



