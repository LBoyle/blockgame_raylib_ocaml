open Constants;
open Raylib;

// Because blocks are a 1d array this will select
// blocks that are wrapped, on the other side of the chunk
let get_block_neighbor_ids = b => {
  (
    b - 1,
    b + 1,
    b - chunkSize,
    b + chunkSize,
    b - chunkSize * chunkSize,
    b + chunkSize * chunkSize,
  );
};

type face_verts_t = (Vector3.t, Vector3.t, Vector3.t, Vector3.t);

type normal_face_vert_t =
  | Top(face_verts_t)
  | Bottom(face_verts_t)
  | InFront(face_verts_t)
  | Behind(face_verts_t)
  | Left(face_verts_t)
  | Right(face_verts_t);

let get_exposed_block_face_verts = (b, chunk: array(Block.t)) => {
  let (behind, ahead, left, right, below, above) = get_block_neighbor_ids(b);
  let (behind, ahead, left, right, below, above) = (
    Utils.array_get_opt(behind, chunk),
    Utils.array_get_opt(ahead, chunk),
    Utils.array_get_opt(left, chunk),
    Utils.array_get_opt(right, chunk),
    Utils.array_get_opt(below, chunk),
    Utils.array_get_opt(above, chunk),
  );
  let (x, y, z) = Utils.index_to_3d_coord(b);
  let back_face =
    if (behind == Some(Air)) {
      Some(
        Behind((
          Vector3.create(x, y +. 1., z),
          Vector3.create(x, y, z),
          Vector3.create(x, y, z +. 1.),
          Vector3.create(x, y +. 1., z +. 1.),
        )),
      );
    } else {
      None;
    };
  let front_face =
    if (ahead == Some(Air)) {
      Some(
        InFront((
          Vector3.create(x +. 1., y, z),
          Vector3.create(x +. 1., y +. 1., z),
          Vector3.create(x +. 1., y +. 1., z +. 1.),
          Vector3.create(x +. 1., y, z +. 1.),
        )),
      );
    } else {
      None;
    };
  let right_face =
    if (right == Some(Air)) {
      Some(
        Right((
          Vector3.create(x, y +. 1., z +. 1.),
          Vector3.create(x, y, z +. 1.),
          Vector3.create(x +. 1., y, z +. 1.),
          Vector3.create(x +. 1., y +. 1., z +. 1.),
        )),
      );
    } else {
      None;
    };
  let left_face =
    if (left == Some(Air)) {
      Some(
        Left((
          Vector3.create(x, y, z),
          Vector3.create(x, y +. 1., z),
          Vector3.create(x +. 1., y +. 1., z),
          Vector3.create(x +. 1., y, z),
        )),
      );
    } else {
      None;
    };
  let bottom_face =
    if (below == Some(Air)) {
      Some(
        Bottom((
          Vector3.create(x, y, z),
          Vector3.create(x, y, z +. 1.),
          Vector3.create(x +. 1., y, z +. 1.),
          Vector3.create(x +. 1., y, z),
        )),
      );
    } else {
      None;
    };
  let top_face =
    if (above == Some(Air)) {
      Some(
        Top((
          Vector3.create(x, y +. 1., z),
          Vector3.create(x, y +. 1., z +. 1.),
          Vector3.create(x +. 1., y +. 1., z +. 1.),
          Vector3.create(x +. 1., y +. 1., z),
        )),
      );
    } else {
      None;
    };
  switch (back_face, front_face, right_face, left_face, bottom_face, top_face) {
  | (None, None, None, None, None, None) => None
  | _ =>
    Some(
      List.filter_map(
        x => x,
        [back_face, front_face, right_face, left_face, bottom_face, top_face],
      ),
    )
  };
};

let gen_chunk_mesh = (_ci: int, chunk: array(Block.t)) => {
  // verts is a bit dense, list(list(normal(tuple(Vector3))))
  // It's a filtered list of blocks,
  // each block is a filtered list of faces,
  // each face is first a direction, then
  // a 4-tuple of Vector3 coords

  let directional_face_verts =
    List.mapi(
      (b, block: Block.t) =>
        if (block == Block.Air) {
          None;
        } else {
          get_exposed_block_face_verts(b, chunk);
        },
      Array.to_list(chunk),
    )
    |> List.filter_map(x => x)
    |> List.flatten;

  let flat_verts_list =
    directional_face_verts
    // Two triangles for each face
    |> List.map(direction =>
         switch (direction) {
         | Top((v1, v2, v3, v4))
         | Bottom((v1, v2, v3, v4))
         | InFront((v1, v2, v3, v4))
         | Behind((v1, v2, v3, v4))
         | Left((v1, v2, v3, v4))
         | Right((v1, v2, v3, v4)) =>
           [
             [
               Vector3.x(v1),
               Vector3.y(v1),
               Vector3.z(v1),
               Vector3.x(v2),
               Vector3.y(v2),
               Vector3.z(v2),
               Vector3.x(v3),
               Vector3.y(v3),
               Vector3.z(v3),
             ],
             [
               Vector3.x(v1),
               Vector3.y(v1),
               Vector3.z(v1),
               Vector3.x(v3),
               Vector3.y(v3),
               Vector3.z(v3),
               Vector3.x(v4),
               Vector3.y(v4),
               Vector3.z(v4),
             ],
           ]
           |> List.flatten
         }
       )
    |> List.flatten;

  // Normals are two unit vectors per square face,
  // one for each triangle
  let flat_normals_list =
    directional_face_verts
    |> List.map(direction =>
         switch (direction) {
         | Top(_) => [0., 1., 0., 0., 1., 0.]
         | Bottom(_) => [0., (-1.), 0., 0., (-1.), 0.]
         | InFront(_) => [1., 0., 0., 1., 0., 0.]
         | Behind(_) => [(-1.), 0., 0., (-1.), 0., 0.]
         | Left(_) => [0., 0., 1., 0., 0., 1.]
         | Right(_) => [0., 0., (-1.), 0., 0., (-1.)]
         }
       )
    |> List.flatten;

  let chunk_mesh = Mesh.create();
  Mesh.set_vertex_count(chunk_mesh, List.length(flat_verts_list) / 3);
  Mesh.set_triangle_count(chunk_mesh, List.length(flat_verts_list) / 9);
  Mesh.set_vertices(
    chunk_mesh,
    Ctypes.CArray.of_list(Ctypes.float, flat_verts_list),
  );
  Mesh.set_normals(
    chunk_mesh,
    Ctypes.CArray.of_list(Ctypes.float, flat_normals_list),
  );

  chunk_mesh;
};

module MeshCache =
  Hashtbl.Make({
    type t = int;
    let equal = Int.equal;
    let hash = Hashtbl.hash;
  });

// module MeshCache =
//   Map.Make({
//     type t = int;
//     let compare = compare;
//   });

let mesh_cache: MeshCache.t(Mesh.t) = MeshCache.create(0);

let get_mesh_for_chunk_opt = ci =>
  switch (MeshCache.find(mesh_cache, ci)) {
  | exception _exn => None
  | chunk_mesh => Some(chunk_mesh)
  };

let generate_mesh_side_effect = ci =>
  switch (World.get_chunk_at_index(ci)) {
  | exception _exn => ()
  | chunk =>
    switch (MeshCache.find(mesh_cache, ci)) {
    | exception _exn =>
      print_endline("Generating chunk " ++ string_of_int(ci));
      let chunk_mesh = gen_chunk_mesh(ci, chunk);
      print_endline("Uploading mesh " ++ string_of_int(ci));
      upload_mesh(addr(chunk_mesh), false);
      print_endline("Caching mesh " ++ string_of_int(ci));
      MeshCache.add(mesh_cache, ci, chunk_mesh);
    | _old_mesh =>
      // let chunk_mesh = gen_chunk_mesh(ci, chunk);
      // upload_mesh(addr(chunk_mesh), false);
      // MeshCache.replace(mesh_cache, ci, chunk_mesh);
      // unload_mesh(old_mesh);
      ()
    }
  };

let get_mesh_for_chunk_at_index = ci => {
  switch (MeshCache.find(mesh_cache, ci)) {
  | chunk_mesh => chunk_mesh
  | exception _exn =>
    // Generates the chunk if missing from the cache
    let chunk = World.get_chunk_at_index(ci);
    let chunk_mesh = gen_chunk_mesh(ci, chunk);
    upload_mesh(addr(chunk_mesh), false);
    MeshCache.add(mesh_cache, ci, chunk_mesh);
    chunk_mesh;
  };
};

let unload_chunk_meshes = () => {
  MeshCache.iter((_, mesh) => unload_mesh(mesh), mesh_cache);
  MeshCache.reset(mesh_cache);
};

let clear_chunks_from_vram = (chunks_to_clear: list(int)) => {
  chunks_to_clear
  |> List.iter(ci => {
       switch (MeshCache.find(mesh_cache, ci)) {
       | chunk_mesh =>
         print_newline();
         print_endline(
           "attempting to unload chunk with id: " ++ string_of_int(ci),
         );

         switch (unload_mesh(chunk_mesh)) {
         | exception exn =>
           print_newline();
           print_endline(
             "Failed to unload mesh with id " ++ string_of_int(ci),
           );
           print_endline(Printexc.to_string(exn));
         | _res =>
           print_endline("Unloaded mesh with id " ++ string_of_int(ci))
         };
       | exception _exn => ()
       }
     });
};
