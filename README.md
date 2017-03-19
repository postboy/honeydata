## Honeydata 0.11 — honey encryption library for numeric data
[Honey encryption](https://en.wikipedia.org/wiki/Honey_Encryption) is a novel type of data encryption. This library provides honey encryption primitives for various numeric data types. Project is distributed under BSD 2-clause license.

## Supported data types
* **integer:** (u)int8_t, (u)int16_t, (u)int32_t, (u)int64_t subsets with uniform and arbitrary distribution
* **floating point:** very small subsets of float and double with uniform distribution
* some drafts of other features

## Warnings
* this is the research project! it's not well tested and can contain serious bugs, plus it don't offers any backward compatibility
* *improper* use of other cryptographic primitives (padding, signatures, message authentication codes, authenticated encryption, encryption modes) can kill honey effect if an attacker will be able to distinguish decoy data and real data
* library is not secure against side-channel attacks
* library is not aware of little/big endian number encoding, so you should remember it before sending encoded data to another computer

## Platform
* GNU/Linux (tested)
* other *nix and Windows (not tested)

## Dependencies
* OpenSSL: `openssl libssl libssl-dev` packages. Tested on 1.0.2g-fips version.
* GNU MP: `libgmp10 libgmp-dev` packages. Tested on 6.1.0 version.

## Contributors
Developer: Zuboff Ivan // anotherdiskmag on gooooooogle mail
