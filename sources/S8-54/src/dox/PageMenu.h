//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*! \page page5 �������� ����
    \tableofcontents

    \section section1 �������� �������� ����

    �������� �������� ���� ����� ��� Page. ��� ���:
    \code
    +------------+-------------+
    |  ��������  | ��������    |
    |  � ����    | ������ ���� |
    +------------+-------------+
    | pDisplay   | �������     |
    | pChanA     | ����� 1     |
    | pChanB     | ����� 2     |
    | pTrig      | �����       |
    | pTime      | ���������   |
    | pCursors   | �������     |
    | pMemory    | ������      |
    | pMeasures  | ���������   |
    | pSevice    | ������      |
    | pHelp      | ������      |
    +------------+-------------+
    \endcode

    \section section2 �������� ������������ �������� ����
    \code
    +-------------+-------------+------------------------------------------------------------------+
    | ��������� � | ���         | ����������                                                       |
    | ��������    | ���������   |                                                                  |
    +-------------+-------------+------------------------------------------------------------------+
    | b           | Button,     |                                                                  |
    |             | SmallButton |                                                                  |
    | c           | Choice      |                                                                  |
    | p, pp, ppp  | Page        | p - ������� ��������, pp - �������� ������� ������ �����������,  |
    |             |             | ppp - �������� ������� ������ �����������                        |
    +-------------+-------------+------------------------------------------------------------------+
    \endcode
    ����� ��� �������� �������� ���������. ��������, \b mcShow - ��������� Choice, ������������� ����� ���� <b>������� - ����������</b>.
    �� ����� "����������" ��������� Show, � "�������" ����� �� ���������� � ��������, ������ ��� �������� ��������� ������� ���������� ���������
    �������� ������.

    \section section3 �������� ������������ ������ ����
    \code
    ������
    +-----------+-------------+-------------------------------------------------------------------------------+
    | ������    | � ����      | ��������                                                                      |
    |           | ��������    |                                                                               |
    +-----------+-------------+-------------------------------------------------------------------------------+
    | Draw      | SmallButton | ������� ���������� ��������� SmallButton                                      |
    | IsActive  | All         | ������� ���������� true, ���� ������� �������. ��������� �� ���� ���������    |
    | OnChanged | Choice      | ������� ���������� ��� ��������� ��������� Choice                             |
    | OnPress   | Button      | ������� �� ������� �������� ���� Button                                       |
    | OnRegSet  | ���������   | ������� �� ������� ����� ���������                                            |
    | OnDraw    | Page        | �������������� �������, ������� ���������� �� ����� ���������                 |
    +-----------+-------------+-------------------------------------------------------------------------------+
    \endcode
*/
