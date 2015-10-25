
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"dbHead.h"


/**
 * @brief ��ѯһ���ļ��Ƿ����
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int queryFileID( struct dbSysHead *head, long fid )
{
	int i;

	for( i=0; i<MAX_FILE_NUM; i++ ) {
		if( (head->desc).fileDesc[i].fileID == fid ){
				break;
		}
	}
	if( MAX_FILE_NUM == i )
		return -1;
	else
		return i;
}


/**
 * @brief ��ʼ��һ��Segment
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] startPage : long     Ϊ��Segment�����������ҳ��ĵ�һ��ҳ���ҳ��
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int initSegment(struct Segment *newSegment, long startPage, long pageNum, long fd, long preAddr, long nextAddr)
{
	long i;
	
	for( i=0; i<pageNum; i++ ){
		(*newSegment).pageNo[i] = startPage + i;
	}
	(*newSegment).count = pageNum;
	(*newSegment).fid = fd;
	(*newSegment).preAddr = preAddr;
	(*newSegment).nextAddr = nextAddr;
	return 0;
}



/**
 * @brief ���һ��Segment��ֵ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] SegmentAddr : long     ��Segment�ڴ洢�ļ��е�λ��
 * @param [out] newSegment :struct Segment *    ���ڽ�����Ӧ��Segment
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int getSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment)
{
	rewind(head->fpdesc);
	fseek(head->fpdesc, segmentAddr, SEEK_SET);
	fread(newSegment, sizeof(struct Segment), 1, head->fpdesc);
	return 0;
}



/**
 * @brief ��һ��Segment����Ϣд��洢�ļ�
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] SegmentAddr : long     ��Segment�ڴ洢�ļ��е�λ��
 * @param [out] newSegment :struct Segment *    Ҫд���Segment����
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int writeSegmentValue(struct dbSysHead *head, long segmentAddr, struct Segment *newSegment)
{
	rewind(head->fpdesc);
	fseek(head->fpdesc, segmentAddr, SEEK_SET);
	fwrite(newSegment, sizeof(struct Segment), 1, head->fpdesc);
	return 0;
}



/**
 * @brief ���һ���ļ������һ��Segment�ڴ洢�ļ��е�λ��
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @return  long
 * @retval  �洢�ļ���λ��
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
long getLastSegmentAddr(struct dbSysHead *head, long fid)
{
	struct Segment newSegment;
	int descNo;
	long segmentAddr;
	long curAddr;

	descNo = queryFileID( head, fid );
	if(descNo<0) {
		return descNo;
	}
	segmentAddr = (head->desc).fileDesc[descNo].fileAddr;
	while (segmentAddr>0) {
		getSegmentValue(head, segmentAddr, &newSegment);
		curAddr = segmentAddr;
		segmentAddr = newSegment.nextAddr;
	}
	return curAddr;
}



/**
 * @brief ��չһ���ļ��Ŀռ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long     �ļ���ʶ
 * @param [in] extendPage : long    Ҫ��չ��ҳ��
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int extendFileSpace( struct dbSysHead *head, long fid, int extendPage )
{
	long segmentAddr;
	struct Segment newSegment;
	struct Segment lastSegment;
	int allocStaPage;
	int i;
	int count;
	int nPage;
	int numPage;
	int idx;

	numPage = extendPage;
	allocStaPage = allocatePage( head, numPage );
	if( allocStaPage < 0 ) {
		printf("extendFileSpace error:out of disk space.");
		exit(0);
	}
	segmentAddr = getLastSegmentAddr(head, fid);
	getSegmentValue(head, segmentAddr, &lastSegment);
	if (lastSegment.count < PAGE_PER_SEGMENT) {
		nPage = (numPage < (PAGE_PER_SEGMENT - lastSegment.count)) ? numPage : (PAGE_PER_SEGMENT - lastSegment.count);
		count = lastSegment.count;
		for( i=0; i<nPage; i++ ) {
			lastSegment.pageNo[count + i] = allocStaPage;
			allocStaPage++;
		 }
		lastSegment.count += nPage;
		writeSegmentValue(head, segmentAddr, &lastSegment);
		numPage -= nPage;
	}
	if( numPage > 0 ) {
		while( numPage > 0 ) {
			nPage = (numPage < PAGE_PER_SEGMENT) ? numPage : PAGE_PER_SEGMENT;
			initSegment(&newSegment, allocStaPage, nPage, fid, segmentAddr, -1);
			allocStaPage += nPage;

			lastSegment.nextAddr = (head->desc).segmentAddr + (head->desc).segmentCur * sizeof(struct Segment);
			newSegment.preAddr = segmentAddr;
			writeSegmentValue(head, segmentAddr, &lastSegment);
			segmentAddr = lastSegment.nextAddr;
			(head->desc).segmentCur++;
			(head->desc).segmentNum++;

			lastSegment.count = newSegment.count;
			lastSegment.nextAddr = newSegment.nextAddr;
			lastSegment.preAddr = newSegment.preAddr;
			for (i = 0; i<newSegment.count; i++) {
				lastSegment.pageNo[i] = newSegment.pageNo[i];
			}
			numPage -= nPage;
		}
		writeSegmentValue(head, segmentAddr, &newSegment);
	}
	idx = queryFileID(head,fid);
	head->desc.fileDesc[idx].filePageNum += extendPage;
	return 0;
}



/**
 * @brief ����һ���ļ���Ϊ������ռ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @return  int 
 * @retval  �����ļ��ı�ʶ�ţ���fid
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
long creatFileSpace( struct dbSysHead *head )
{
	struct Segment newSegment;
	long segmentAddr;
	long alloStaPage;
	int i;

	if( (head->desc).curFileNum >= MAX_FILE_NUM ) {
			printf("creatFile Space error:total files cannot be more than %d.\n",(head->desc).curFileNum);
			return -1;
	}
	segmentAddr = (head->desc).segmentAddr + sizeof(struct Segment) * (head->desc).segmentCur;
	alloStaPage = allocatePage( head, PAGE_PER_SEGMENT );
	if( alloStaPage >= 0 ) {
			for( i=0; i<MAX_FILE_NUM; i++ ) {
				if( (head->desc).fileDesc[i].fileID <= 0 ) {
					break;
				}
			}
			(head->desc).fileDesc[i].fileID = (head->desc).curfid;
			(head->desc).fileDesc[i].fileAddr = segmentAddr;
			(head->desc).fileDesc[i].filePageNum = PAGE_PER_SEGMENT;
			(head->desc).curFileNum++;
			initSegment( &newSegment, alloStaPage, PAGE_PER_SEGMENT, (head->desc).fileDesc[i].fileID, -1, -1 );
			rewind(head->fpdesc);
			fseek( head->fpdesc,  (head->desc).fileDesc[i].fileAddr, SEEK_SET );
			fwrite( &newSegment, sizeof(struct Segment), 1, head->fpdesc );
			(head->desc).segmentNum++;
			(head->desc).segmentCur++;
			(head->desc).curfid++;
	}
	else {
			printf("creatFileSpace error:creat error");
			exit(0);
	}
	return (head->desc).fileDesc[i].fileID;
}



/**
 * @brief Ϊһ���ļ����������Ŀռ䣬���û���㹻�������ռ䣬�򲻷���
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] reqPageNum : int    ��������ҳ��
 * @return  long 
 * @retval  �������ɹ����򷵻ظ������ռ�ĵ�һ��ҳ���ҳ��
 *
 * @author mengxi	
 * @date 2015/10/20 
 **/
long allocatePage( struct dbSysHead *head, int reqPageNum )
{
	long totalPage;
	int i,j;
	int page,pos;
	int count;
	int alloStaPage;

	totalPage = head->desc.totalPage;
	for( i=0; i<totalPage; i++ ) {
			page = i/(8*sizeof(long));
			pos = i - 8*sizeof(long)*page + 1;
			count = 0;
			if( getBit( *(head->bitMap+page),pos) == P_AVAI ) {
				for( j=i; count<reqPageNum && j<totalPage; j++ ) {
						page = j/(8*sizeof(long));
						pos = j - 8*sizeof(long)*page + 1;
						if( getBit( *(head->bitMap+page),pos) == P_AVAI ) {
							count++;
						}
						else {
							break;
						}
				}
			}
			if( count == reqPageNum ) {
				break;
			}
			else {
				i = i + count;
			}
	}
	if( count != reqPageNum ) {
		return ALLO_FAIL;
	}
	else {
		alloStaPage = i;
		for( j=0; j<reqPageNum; j++ ) {
			page = (i+j) / ( 8*sizeof(long) );
			pos = (i+j) - 8*sizeof(long)*page + 1;
			setBit( head->bitMap+page, pos, P_UNAVAI );
		}
		(head->desc).pageAvai -= reqPageNum;
	}
	return alloStaPage;
}



/**
 * @brief ����һ��ҳ
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] pageNo : int    Ҫ���յ�ҳ��
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int recyOnePage( struct dbSysHead *head ,long pageNo )
{
	int page,pos;

	page = pageNo / ( sizeof(long) * 8 );
	pos = pageNo - page*8*sizeof(long) + 1;
	setBit( head->bitMap+page, pos, P_AVAI );
	return 0;
}



/**
 * @brief ���շ����һ���ļ�������ҳ
 *
 * @param [in] head  : struct dbSysHead *
 * @param [in] SegmentAddr : long    ���ļ��ĵ�һ��Segment�ڴ洢�ļ����׵�ַ
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int recyPage(struct dbSysHead *head, long segmentAddr)
{
	struct Segment newSegment;
	int i;

	getSegmentValue(head, segmentAddr, &newSegment);
	for (i = 0; i<newSegment.count; i++) {
		recyOnePage(head, *(newSegment.pageNo + i));
	}
	(head->desc).segmentNum--;
	while(newSegment.nextAddr > 0) {
		getSegmentValue(head, newSegment.nextAddr, &newSegment);
		for (i = 0; i<newSegment.count; i++) {
			recyOnePage(head, *(newSegment.pageNo + i));
		}
		(head->desc).segmentNum--;
	}
	return 0;
}



/**
 * @brief ɾ��һ���ļ�
 *
 * @param [in/out] head  : struct dbSysHead *
 * @param [in] fid : long    Ҫ���յ��ļ��ı�ʶ
 * @return  int 
 *
 * @author mengxi
 * @date 2015/10/20 
 **/
int recyFileSpace( struct dbSysHead *head, long fid )
{
	int i;

	i =  queryFileID(head,fid);
	if( i < 0 ) {
		return i;
	}
	recyPage( head, (head->desc).fileDesc[i].fileAddr );
	(head->desc).pageAvai += (head->desc).fileDesc[i].filePageNum;
	(head->desc).fileDesc[i].fileAddr = 0;
	(head->desc).fileDesc[i].fileID = 0;
	(head->desc).fileDesc[i].filePageNum = 0;
	(head->desc).curFileNum--;
	return 0;
}