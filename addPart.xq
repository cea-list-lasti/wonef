CREATE DB JAWS INPUT
OPEN JAWS
XQUERY for $synset in //SYNSET let $id := data($synset/@id) let $def := $synset/ORIGINALDEF let $insts := $synset/INSTANCES[INSTANCE/@processed="MODULE"] let $instsProcessed := ( for $inst in $insts/INSTANCE[@processed="MODULE"] let $trans := data($insts/@translation) return <INSTANCES translation="{$trans}">{$inst}</INSTANCES> ) let $candOrig := ( for $inst in $insts/INSTANCE[@processed="MODULE"] let $orig := data($inst/@original) let $cand := $synset/CANDIDATES[@original=$orig] return $cand ) return ( if ($insts) then <SYNSET id="{$id}">{$def}{$candOrig}{$instsProcessed}</SYNSET> else <SYNSET id="{$id}" /> )
DROP DB JAWS
