for $synset in //SYNSET
	let $id := data($synset/@id)
	let $def := $synset/ORIGINALDEF
	let $cand := $synset/CANDIDATES[@processed="RELATION"]
	let $orig := data($cand/@original)
	let $inst := $synset/INSTANCE
	let $instProcessed := (
		for $o in $orig
			return (
				for $i in $inst
					where matches (data($i/@original), $o)
					return <INSTANCE original="{$o},">{data($i)}</INSTANCE>)
	)
	return
		if ($cand)
		then <SYNSET id="{$id}">{$def}{$cand}{$instProcessed}</SYNSET>
		else rien