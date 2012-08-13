-module(geef).
-export([start/0, hex_to_raw/1, object_exists/2, repository/1, repository_path/1]).
-on_load(start/0).

start() ->
    case code:priv_dir(geef) of
        {error, bad_name} ->
            SoName = filename:join("priv", geef);
        Dir ->
            SoName = filename:join(Dir, geef)
    end,
    io:format("Hey There: ~s~n", [SoName]),
    erlang:load_nif(SoName, 0).

hex_to_raw(_Val) ->
    nif_error(?LINE).    

object_exists(_Val, _Val2) ->
    nif_error(?LINE).

repository_open(_Val) ->
    nif_error(?LINE).

repository_get_path(_Val) ->
    nif_error(?LINE).

repository_path({repository, Handle}) ->
    repository_get_path(Handle).

repository(Path) ->
    {ok, Repository} = repository_open(Path),
    {repository, Repository}.

nif_error(Line) ->
    exit({nif_not_loaded,module,?MODULE,line,Line}).
