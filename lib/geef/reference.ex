defrecord Geef.Reference, Record.extract(:geef_reference, from: "src/geef_records.hrl") do
  alias Geef.Repository

  def lookup(repo, name) do
    case :geef_ref.lookup(repo, name) do
      {:ok, ref} ->
        {:ok, Geef.Reference.new ref}
      error ->
        error
    end
  end

  def lookup!(repo, name) do
    case lookup(repo, name) do
      {:ok, ref} ->
        ref
      {:error, err} ->
        raise err
    end
  end

  def resolve(ref = Geef.Reference[]) do
    case :geef_ref.resolve(set_elem(ref, 0, :geef_reference)) do
      {:ok, ref} ->
        {:ok, Geef.Reference.new ref}
      error ->
        error
    end
  end

  def resolve!(ref = Geef.Reference[]) do
    case resolve(ref) do
      {:ok, resolved} ->
        resolved
      {:error, err} ->
        raise err
    end
  end

end