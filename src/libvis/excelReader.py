import xlrd

class ExcelReader:
    # ----------------------------------------------------
    # define local values
    # ----------------------------------------------------
    sheetIndex = 0
    fileName = "/home/pi/map.xls"
    rowCount = 0
    columnCount = 0

    excelStart = 3

    def __init__(self):
        workbook = xlrd.open_workbook(self.fileName)
        sheet = workbook.sheet_by_index(0)

        # first check if we got a workbook and a sheet
        if workbook is None:
            exit(-1)

        if sheet is None:
            exit(-1)

        # find the last index in row and colum
        self.rowCount = sheet.cell(1, 0)
        self.columnCount = sheet.cell(1, 1)
        print "ROWS = " + str(self.rowCount)
        print "COLUMNS = " + str(self.columnCount)

        f = open('/var/www/html/map.html', 'w')

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

        # read data
        for row in range(sheet.nrows):

            complete += lineStart
            for col in sheet.row_values(row):
                if col.strip() == '1':
                    complete += border
                else:
                    complete += free

            complete += lineEnd

        complete += footText
        print complete

        f.write(complete)
        f.close()

xls = ExcelReader()