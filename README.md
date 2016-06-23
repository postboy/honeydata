## Honeydata 0.04 — honey encryption library for numeric data
[Honey encryption] (https://en.wikipedia.org/wiki/Honey_Encryption) is a novel type of data encryption. This library provides honey encryption primitives for various numeric data types. Licence: [BSD 2-clause] (http://opensource.org/licenses/bsd-license.php) with public domain parts.

## Supported data types
* uint8_t, int8_t, uint16_t subsets distributed uniformly
* more is cooking...

## Warnings
* this is the research project! it's not well tested and can contain serious bugs, plus it don't offers any backward compatibility
* library is not aware of little/big endian number encoding, so you should remember it before sending encoded data to another computer
* *improper* use of other cryptographic primitives (padding, signatures, HMACs, authenticated encryption, encryption modes) can kill honey effect if an attacker will be able to distinguish decoy data and real data

## Platform
* GNU/Linux (tested)
* other *nix and Windows (not tested)

## Dependencies
OpenSSL: `openssl libssl libssl-dev` packages. Tested on 1.0.1f version.

## Contributors
Developer: Zuboff Ivan // anotherdiskmag on gooooooogle mail
