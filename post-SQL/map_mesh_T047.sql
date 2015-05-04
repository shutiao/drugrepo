Create Table test.GSE1_Disease
SELECT
    map.GSE, CUI_T047.CUI, CUI_T047.SAB, CUI_T047.CODE, CUI_T047.STR
FROM
    test.GSE1 map
    
        INNER JOIN	test.CUI_T047
		ON map.MESH = CUI_T047.STR
union

select distinct
    map.GSE, mrconso_msh.CUI, mrconso_ICD9CM.SAB,mrconso_ICD9CM.CODE, mrconso_ICD9CM.STR

FROM
    test.GSE1 map
        INNER JOIN
    umls.mrconso mrconso_msh ON map.MESH = mrconso_msh.STR
        AND mrconso_msh.SAB = 'MSH'
        INNER JOIN        
    umls.mrconso mrconso_ICD9CM ON mrconso_msh.CUI = mrconso_ICD9CM.CUI
        AND mrconso_ICD9CM.SAB = 'ICD9CM'
