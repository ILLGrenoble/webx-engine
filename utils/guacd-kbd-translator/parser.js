import * as fs from 'fs';

export const modifiers = {
    SHIFT_MODIFIER_KEY: 'WEBX_SHIFT_MODIFIER_KEY',
    ALTGR_MODIFIER_KEY: 'WEBX_ALTGR_MODIFIER_KEY',
    CAPS_LOCK_KEY: 'WEBX_CAPS_LOCK_KEY',
    NUM_LOCK_KEY: 'WEBX_NUM_LOCK_KEY'
} 


const convertToModifier = (text) => {
    // if (text === 'caps') {
    //     return {isLock: true, value: modifiers.CAPS_LOCK_KEY};
    // } else 
    if (text === 'num') {
        return {isLock: true, value: modifiers.NUM_LOCK_KEY};
    } else if (text === 'shift') {
        return {isLock: false, value: modifiers.SHIFT_MODIFIER_KEY};
    } else if (text === 'altgr') {
        return {isLock: false, value: modifiers.ALTGR_MODIFIER_KEY};
    }
}

const guacdSymbols = [{
    layout: 'gb',
    name: 'en_gb_qwerty',
    inherits: ['base']
}, {
    layout: 'hu',
    name: 'hu_hu_qwertz',
    inherits: ['base']
}, {
    layout: 'br',
    name: 'pt_br_qwerty',
    inherits: ['base']
}, {
    layout: 'dk',
    name: 'da_dk_qwerty',
    inherits: ['base']
}, {
    layout: 'us',
    name: 'en_us_qwerty',
    inherits: ['base']
}, {
    layout: 'be',
    name: 'fr_be_azerty',
    inherits: ['base']
}, {
    layout: 'it',
    name: 'it_it_qwerty',
    inherits: ['base']
}, {
    layout: 'se',
    name: 'sv_se_qwerty',
    inherits: ['base']
}, {
    layout: 'ch(de)',
    name: 'de_ch_qwertz',
    inherits: ['base']
}, {
    layout: 'es',
    name: 'es_es_qwerty',
    inherits: ['base']
}, {
    layout: 'ch(fr)',
    name: 'fr_ch_qwertz',
    inherits: ['base']
}, {
    layout: 'jp',
    name: 'ja_jp_qwerty',
    inherits: ['base']
}, {
    layout: 'tr',
    name: 'tr_tr_qwerty',
    inherits: ['base']
}, {
    layout: 'de',
    name: 'de_de_qwertz',
    inherits: ['base']
}, {
    layout: 'latam',
    name: 'es_latam_qwerty',
    inherits: ['base']
}, {
    layout: 'fr',
    name: 'fr_fr_azerty',
    inherits: ['base']
}, {
    layout: 'no',
    name: 'no_no_qwerty',
    inherits: ['base']
}];

const parseSymbolFile = (file) => {
    console.log(`Parsing ${file}`);
    const data = fs.readFileSync(`mappings/${file}`, 'utf8');

    return data.split('\n')
        .filter(s => s.length > 0)
        .map(line => {
            const pattern = /^\s*map\s+([^~]*)\s+~\s+(".*"|0x[0-9A-Fa-f]+)\s*(?:#.*)?$/;
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        })
        .filter(matches => matches != null)
        .flatMap(matches => {
            const options = matches[1].split(/\s+/);
            const onto = matches[2];
            const modifiersSet = [];
            const locksSet = [];
            const modifiersClear = [];
            const locksClear = [];
            const rdpScancodes = [];
            const charVals = [];

            // Handle options
            for (const option of options) {
                let valueExpr;
                if (valueExpr = new RegExp(/^(-|\+)([a-z]+)/, 'g').exec(option)) {
                    const plusOrMinus = valueExpr[1];
                    const modifierType = valueExpr[2];
                    const modifier = convertToModifier(modifierType);
                    if (modifier) {
                        if (plusOrMinus === '+' && modifier.isLock) {
                            locksSet.push(modifier.value);
                        
                        } else if (plusOrMinus === '-' && modifier.isLock) {
                            locksClear.push(modifier.value);
                        
                        } else if (plusOrMinus === '+' && !modifier.isLock) {
                            modifiersSet.push(modifier.value);
                        
                        } else if (plusOrMinus === '-' && !modifier.isLock) {
                            modifiersClear.push(modifier.value);
                        }
                    }

                } else if (valueExpr = new RegExp(/^(0x[0-9A-Fa-f]+)$/, 'g').exec(option)) {
                    const rdpScancode = parseInt(option);
                    rdpScancodes.push(rdpScancode);

                } else if (valueExpr = new RegExp(/^(0x[0-9A-Fa-f]+)\.\.(0x[0-9A-Fa-f]+)$/, 'g').exec(option)) {
                    const rdpScancodeStart = parseInt(valueExpr[1]);
                    const rdpScancodeEnd = parseInt(valueExpr[2]);
                    for (let i = rdpScancodeStart; i <= rdpScancodeEnd; i++) {
                        rdpScancodes.push(i);
                    }
                }
            }

            // Handle onto
            if (new RegExp(/^0x/, 'g').exec(onto)) {
                charVals.push(parseInt(onto));

            } else {
                for (let i = 1; i < onto.length - 1; i++) {
                    let ascii = onto.charCodeAt(i);
                    // if (ascii >= 0x0100) {
                        // ascii |= 0x01000000;
                    // }

                    charVals.push(ascii);
                }
            }
            
            // Check number of keysyms and rdpScancodes
            if (charVals.length !== rdpScancodes.length) {
                console.error(`Keysym and RDP scan code lengths differ for line '${matches[0]}' in file '${file}'`);
                return null
            }

            return charVals.map((charVal, index) => {
                return {charVal, rdpScancode: rdpScancodes[index], modifiersSet, locksSet, modifiersClear, locksClear};
            });
        })
        .filter(parse => parse != null);
}


export const parseSymbols = () => {
    const parsed = guacdSymbols.map(guacdSymbol => {
        const { layout, name, inherits } = guacdSymbol;
        const file = `${name}.keymap`;
        const symbols = parseSymbolFile(file);
    
        if (inherits && inherits.length > 0) {
            inherits.forEach(inherited => {

                const inheritedFile = `${inherited}.keymap`;

                parseSymbolFile(inheritedFile).forEach(key => {
                    symbols.push(key);      
                });
            });
        }
    
        return { layout, name, symbols }
    });

    return parsed;
}

export const parseKeySymDefFile = () => {
    const data = fs.readFileSync(`mappings/keysymdef.h`, 'utf8');
    return data.split('\n')
        .filter(s => s.length > 0)
        .map(line => {
            const pattern = /#define XK_(\w+)\s+(0x\w+)(?:.*U\+(\w+))?/;
            const expression = new RegExp(pattern, 'g');
            return expression.exec(line);
        })
        .filter(matches => matches != null)
        .map(matches => {
            const name = matches[1];
            const keysymHex = matches[2];
            const unicode = matches[3];
            const value = parseInt(keysymHex);
            return { name, value, unicode: unicode ? parseInt(`0x${unicode}`) : null };
        });
}