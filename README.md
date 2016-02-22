## Honeydata 0.01 — honey encryption library (work in progress)
[Honey encryption] (https://en.wikipedia.org/wiki/Honey_Encryption) is a novel type of data encryption. This library provides honey encryption primitives for certain types of numeric data. Licence: [BSD 2-clause] (http://opensource.org/licenses/bsd-license.php) with public domain parts.

## Features
* honey encryption of uint8_t subsets distributed uniformly
* more is cooking...

## Warnings
* no backward compatibility at the moment
* library is not aware of little/big endian number encoding, so you should remember it before sending encoded data to another computer
* *improper* use of other cryptographic primitives (padding, signatures, HMACs, authenticated encryption, encryption modes) can kill honey effect, because attacker will be able to distinguish decoy data and real data

## Platform
* GNU/Linux (tested)
* other *nix and Windows (not tested)

## Dependencies
OpenSSL: `openssl libssl libssl-dev` packages. Tested on 1.0.1f version.

## Contributors
Developer: Zuboff Ivan // anotherdiskmag on gooooooogle mail
