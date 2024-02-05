let generate_chunk = () => {
  Array.make(16 * 16, Block.Air)
  |> Array.append(Array.make(16 * 16, Block.Grass))
  |> Array.append(Array.make(16 * 16, Block.Dirt));
};

let initial_blocks = () => {
  Array.make(16 * 16, Block.Air)
  |> Array.append(Array.make(16 * 16, Block.Grass))
  |> Array.append(Array.make(16 * 16, Block.Dirt));
};
