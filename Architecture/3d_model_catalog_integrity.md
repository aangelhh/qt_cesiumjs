# 3D Model Catalog Integrity

## Purpose

`Data/config3DModel.yaml` is the source of truth for the models offered by the
Add Entity dialog. Every configured entry must resolve to a local visual asset.
The catalog loader must not silently hide a platform because its variant-specific
GLB is unavailable.

## Visual compatibility

Each loaded `ModelCatalogEntry` records two paths:

- `configuredRelativePath`: the variant-specific asset requested by the catalog.
- `relativePath`: the asset actually resolved by the loader.

`visualModelCompatibility` describes the result:

- `exact`: the configured asset exists and is used directly.
- `family`: the configured asset is unavailable and the group's declared
  `visualFallbackUrl` is used.

The logical model name and DIS entity type are preserved when a family visual is
used. Visual compatibility is independent from `dynamicsModelCompatibility`;
sharing a GLB does not imply sharing an exact FDM.

## Declared family fallbacks

| Family | Shared visual asset |
| --- | --- |
| Eurofighter Typhoon | `eurofighter_typhoon.glb` |
| Mirage 2000 | `dassault_mirage_2000.glb` |
| Rafale | `dassault_rafale.glb` |
| MiG-29 | `mig-29_fighter.glb` |
| Mirage F1 | `dassault_mirage_f1.glb` |
| Mirage III | `dassault_mirage_iii.glb` |
| C-130 Hercules | `c-130j_super_hercules.glb` |
| CH-47 Chinook | `boeing_ch-47_chinook.glb` |

## Adding external models

Prefer an exact variant GLB when its geometry materially differs from the family
asset. Before adding an external model, record its source, author, license,
required attribution, and any redistribution restrictions. Do not add an asset
whose provenance or redistribution rights cannot be verified.

The catalog integrity test requires every configured model to resolve to an
existing exact or declared family asset. A broken new path therefore fails the
test instead of disappearing from the UI.
