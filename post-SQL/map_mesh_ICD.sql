Create Table test.GSE1_ICD9CM
select distinct
    map.GSE, mrconso_msh.CUI, mrconso_ICD9CM.SAB,mrconso_ICD9CM.CODE, mrconso_ICD9CM.STR

FROM
    test.GSE1nnn map
        INNER JOIN
    umls.mrconso mrconso_msh ON map.MESH = mrconso_msh.STR
        AND mrconso_msh.SAB = 'MSH'
        INNER JOIN
        
    umls.mrconso mrconso_ICD9CM ON mrconso_msh.CUI = mrconso_ICD9CM.CUI
        AND mrconso_ICD9CM.SAB = 'ICD9CM'