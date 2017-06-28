import csv

class csvReader:
    # ----------------------------------------------------
    # define local values
    # ----------------------------------------------------
    sheetIndex = 0
    fileName = 'map.csv'
    rowCount = 0
    columnCount = 0

    excelStart = 3

    data = []

    def __init__(self):
        inputfile = open(self.fileName, "rb")
        workbook = csv.reader(inputfile, delimiter=",")

        # first check if we got a workbook and a sheet
        if workbook is None:
            exit(-1)

        # find the last index in row and colum
        self.data = list(workbook)
        self.rowCount = len(self.data)
        self.columnCount = len(self.data[0])
        print "ROWS = " + str(self.rowCount)
        print "COLUMNS = " + str(self.columnCount)

        htmlf = open('map.html', 'w')

        lineStart = """<tr>"""
        lineEnd = "</tr>"
        border = """<th width="10" height="10" style="background-color:red;"> </th>"""
        free = """<th width="10" height="10"> </th>"""

        startText = """<html>
                <head></head>
                <body><table border="1">"""

        footText = """</table><script>setTimeout("location.reload(true);", 100);</script></body>
                </html>"""

        complete = startText

        for row in self.data:
            complete += lineStart
            print row
            for col in row:
                if col.strip() == '1':
                    complete += border
                else:
                    complete += free

            complete += lineEnd

        complete += footText
        print complete

        htmlf.write(complete)
        htmlf.close()

xls = csvReader()
