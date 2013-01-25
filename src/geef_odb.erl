-module(geef_odb).
-export([exists/2]).

-include("geef_records.hlr").

-spec exists(odb(), string()) -> boolean().
exists(#odb{handle=Handle}, Sha) ->
    geef:odb_object_exists(Handle, Sha).
