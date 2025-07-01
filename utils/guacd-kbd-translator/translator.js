import dedent from 'dedent';

const toHex = (number) => {
    let hex = number.toString(16).toUpperCase();
    if (hex.length < 2) {
         hex = "0" + hex;
    }
    return hex;
}

export const translate = (symbolMappings) => {

    let output = dedent`
        // Automatically generated in utils/xkb-interpreter
        
        #ifndef WEBX_KEYBOARD_SYMBOL_MAPPING_H
        #define WEBX_KEYBOARD_SYMBOL_MAPPING_H
        
        #include <vector>
        #include <map>
        #include "WebXKeyboardMapping.h"
    `;

    const keyMaps = [];

    output += symbolMappings.map(symbolMapping => {
        const layout = symbolMapping.layout;
        const name = symbolMapping.name;
        const keysymMappings = symbolMapping.symbols;

        const webXKeySymsName = `__webx_keysyms_${name.replaceAll('-', '_')}`;
        keyMaps.push({layout: layout, name: name, keySymsName: webXKeySymsName});

        let layoutSymbolsText = dedent`\n
            static std::map<int, WebXKeySymDefinition> ${webXKeySymsName} = {\n
        `;

        layoutSymbolsText += keysymMappings.map(keysymMapping => {
            let mapping = `    { 0x${toHex(keysymMapping.keysymValue)}, { "${keysymMapping.keysymName}", 0x${toHex(keysymMapping.keysymValue)}, 0x${toHex(keysymMapping.keycode)}, 0`;
            if (keysymMapping.modifiersSet.length > 0) {
                mapping += ` | ${keysymMapping.modifiersSet.join(' | ')}`;
            }
            mapping += ', 0';
            if (keysymMapping.modifiersClear.length > 0) {
                mapping += ` | ${keysymMapping.modifiersClear.join(' | ')}`;
            }
            mapping += ', 0';
            if (keysymMapping.locksSet.length > 0) {
                mapping += ` | ${keysymMapping.locksSet.join(' | ')}`;
            }

            mapping += ' } }';

            return mapping;
        }).join(',\n');

        layoutSymbolsText += dedent`
            \n};
        `;


        return layoutSymbolsText;
    }).join('');


    output += dedent`\n
        static std::vector<WebXKeyboardMapping> WEBX_KEY_MAPS = {\n
        `;


    output += keyMaps.map(keymap => {
        return `    { "${keymap.name}", "${keymap.layout}", ${keymap.keySymsName} }`;
    }).join(',\n');

    output += dedent`
        \n};

        #endif /* WEBX_KEYBOARD_SYMBOL_MAPPING_H */
    `;

    return output;

}