(lang dune 3.10)

(name blockgame_raylib)

(generate_opam_files true)

(source
 (github LBoyle/blockgame_raylib))

(authors "LBoyle")

(maintainers "lboyle@protonmail.com")

(license LICENSE)

(package
 (name blockgame_raylib)
 (synopsis "Codename Blockgame")
 (description "A block-game")
 (depends ocaml dune raylib raygui)
 (tags
  ("ReasonML" "Raylib" "2D" "Block Game")))
