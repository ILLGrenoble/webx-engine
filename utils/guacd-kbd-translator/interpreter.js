import { evdevKeycodes } from "./keycodes.js";

export const interpret = (symbols, keysymDefs) => {
  return symbols.map(({ layout, name, symbols }) => {

    console.log(`Interpreting ${name}`);
    const interpreted = symbols.map(({charVal, rdpScancode, ...keysymExtra}) => {

      // Convert keysm/unicode to X11 keysym
      const keysymEntry = keysymDefs.find(keysymDef => {

        // If keysym def has a unicode value, match that to the charVal of the text character.
        // Otherwise, just use the charVal as a keysym value
        if (keysymDef.unicode) {
          return keysymDef.unicode === charVal;
        
        } else {
          return keysymDef.value === charVal;
        }

      });

      if (keysymEntry == null) {
        console.warn(`Could not find keysym for charVal ${charVal} of layout ${name}`);
        return null;
      }

      // Convert RDP scancode to X11 keycode
      if (rdpScancode >= evdevKeycodes.length) {
        console.warn(`RDP scancode ${rdpScancode} for charVal ${charVal} of layout ${name} is too big`);
        return null;
      }

      const keycode = evdevKeycodes[rdpScancode];

      if (keycode === 0){
        console.warn(`Ignoring keysym for charVal ${charVal} of layout ${name} (keycode is 0)`);
        return null;
      } 

      return {keysymValue: keysymEntry.value, keysymName: keysymEntry.name, keycode, charVal, ...keysymExtra};

    }).filter(entry => entry != null);


    return {layout, name, symbols: interpreted};
  });

};