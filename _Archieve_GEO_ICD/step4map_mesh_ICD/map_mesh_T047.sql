Create View test.intermediate2T047 as
SELECT distinct
    GSE1nnn.GSE, mrconso.CUI, mrconso.SAB, mrconso.CODE, mrconso.STR
FROM
    test.GSE1nnn
        INNER JOIN
    umls.mrconso ON GSE1nnn.MESH = mrconso.STR
        AND mrconso.SAB = 'MSH'
