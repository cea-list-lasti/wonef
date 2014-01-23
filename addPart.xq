CREATE DB JAWS INPUT
OPEN JAWS
XQUERY for $synset in //SYNSET let $id := data($synset/@id) let $def := $synset/ORIGINALDEF let $insts_tab := $synset/INSTANCES[INSTANCE/@processed="MODULE"] let $instsProcessed := ( for $insts in $insts_tab let $translation := $insts/@translation let $inst_tab := $insts/INSTANCE[@processed="MODULE"] return <INSTANCES translation="{$translation}">{$inst_tab}</INSTANCES> ) let $orig_tab := distinct-values( for $inst in $insts_tab/INSTANCE[@processed="MODULE"] let $orig := data($inst/@original) return $orig ) let $candOrig := ( for $orig in $orig_tab return $synset/CANDIDATES[@original=$orig] ) return ( if ($insts_tab) then <SYNSET id="{$id}">{$def}{$candOrig}{$instsProcessed}</SYNSET> else <SYNSET id="{$id}" /> )
DROP DB JAWS
