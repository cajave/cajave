//
//      This file is automatically executed when IDA is started.
//      You can define your own IDC functions and assign hotkeys to them.
//
//      You may add your frequently used functions here and they will
//      be always available.
//
//
#include <idc.idc>
//��ת����괦�����Ŀ�ͷ��
static GotoCursorFuncStart()
{
   auto startaddr;
   startaddr=GetFunctionAttr(ScreenEA(),FUNCATTR_START);
   Jump(startaddr);
}
//��ת����괦�����Ľ�β��
static GotoCursorFuncEnd()
{
   auto endaddr;
   endaddr=GetFunctionAttr(ScreenEA(),FUNCATTR_END);
   endaddr=FindCode(endaddr,SEARCH_UP);
   Jump(endaddr);
}

//�о�ָ�����������б����ǿ⺯��,�����������Ҷ�ڵ�ǿ⺯�����ͼ���ȫ��Ҷ����������
static ListCallees(func,arridCallees,arridCnt,arridListedFlag,arridAddedFlag)
{    
      auto name,end,inst,xref,target,calleescnt,ListedCnt,bLeaf; 	
      name=Name(func);
      end=GetFunctionAttr(func,FUNCATTR_END);
      calleescnt=0;
      bLeaf=1;

      //if(name!=-1)
         //Message("------ListCallees func %s------\n",name);//��ѭ��������һֱruning

      if(GetArrayElement(AR_LONG,arridListedFlag,func)==1)
          return;
      else       
          SetArrayLong(arridListedFlag,func,1);//���ú������Ϊ��List



      //����һ��ָ���ǲ���jmp �����ָ��
      inst=func;
      for(target=Dfirst(inst); target!=BADADDR; target=Dnext(inst,target))//������һ��ָ���from me���������б�
      {             
         xref=XrefType();
         if(xref==dr_R)
         {
            if(GetMnem(inst)=="jmp")
            {
                //����Ƿ��ǵ�����еĺ���
                if(!isLoaded(target))
                {
                    bLeaf=0;
                    return;
                } 
            }               
         } 
      }      

      
      for(inst=func; inst<end; inst=FindCode(inst,SEARCH_DOWN|SEARCH_NEXT))//�����ú���������ָ��
      {        
         for(target=Rfirst(inst); target!=BADADDR; target=Rnext(inst,target))//����ÿ��ָ���from me���������б�
         {             
            xref=XrefType();     
 	    if(xref==fl_JN || xref==fl_JF) //�ҵ���jmp��ת����
            {

                   
                   if(GetFunctionAttr(target,FUNCATTR_START) == target && target!=func)//��jmpָ��ǡ�������ⲿ������һ������
                   {
                       auto jmpfuncname;
                       jmpfuncname=Name(target);           
                       if( !(GetFunctionFlags(target) & FUNC_LIB) && strstr(jmpfuncname,"j_")!=0  && strstr(jmpfuncname,"xy")!=0 && strstr(jmpfuncname,"XY")!=0 && strstr(jmpfuncname,"Xy")!=0 )
                       {

                           bLeaf=0;
		           ListCallees(target,arridCallees,arridCnt,arridListedFlag,arridAddedFlag); 
                       }
                   }
            }                     
            else if(xref==fl_CN || xref==fl_CF) //�ҵ���һ�����õ�Ŀ�꺯��
	    {
               auto tgtname;        
               //������õĲ�������Ҳ���ǿ⺯��,Ҳ����xyǰ׺��ͷ���û���������Զ��庯�� 
               tgtname=Name(target);       
               if( target!=func && !(GetFunctionFlags(target) & FUNC_LIB) && strstr(tgtname,"j_")!=0  && strstr(tgtname,"xy")!=0 && strstr(tgtname,"XY")!=0 && strstr(tgtname,"Xy")!=0 )
               {
                   calleescnt++;
                   bLeaf=0;
                   //Message("----called %s at %d----\n",name,inst);   
                   ListCallees(target,arridCallees,arridCnt,arridListedFlag,arridAddedFlag);
               }
  
	    } 
         }
      }
      if(bLeaf)//�����������Ϊ�Ѽ���
      {
         if(GetArrayElement(AR_LONG,arridAddedFlag,func)==1)
            return;
         else
         {
            auto cnt;
            //Message("%s is a leaf func-------------------------------------------\n",name);
            cnt=GetArrayElement(AR_LONG,arridCnt,0);
            SetArrayLong(arridCallees,cnt,func);
            cnt++;
            SetArrayLong(arridCnt,0,cnt);

            SetArrayLong(arridAddedFlag,func,1);//���ú������Ϊ��Add
         }

      }           
}
//ö�ٹ�괦�������յ��õ����зǿ⡢δ��������Ҷ�ڵ㺯��
static ListLeafFuns()
{
   auto func,name;
   func=GetFunctionAttr(ScreenEA(),FUNCATTR_START);
   name=Name(func);
   if(func!=-1)
   {
        auto arridCallees,arridCnt,arridListedFlag,arridAddedFlag,cnt,i,subcnt;//callees�����ȫ�ֱ���Ԫ�ظ���
        arridCallees=GetArrayId("xy_callees");
        if(arridCallees!=-1)
        {
           DeleteArray(arridCallees);
        } 
        arridCallees=CreateArray("xy_callees");

        arridCnt=GetArrayId("xy_cnt");
        if(arridCnt!=-1)
        {
           DeleteArray(arridCnt);
        }
        arridCnt=CreateArray("xy_cnt");
        SetArrayLong(arridCnt,0,0);//cnt1=0,ָ��ȥ�ظ����Ҷ�ڵ㺯������


        arridListedFlag=GetArrayId("xy_ListedFlag");
        if(arridListedFlag!=-1)
        {
           DeleteArray(arridListedFlag);
        }
        arridListedFlag=CreateArray("xy_ListedFlag");//��List���


        arridAddedFlag=GetArrayId("xy_AddedFlag");
        if(arridAddedFlag!=-1)
        {
           DeleteArray(arridAddedFlag);
        }
        arridAddedFlag=CreateArray("xy_AddedFlag");//��Add���

       
	ListCallees(func,arridCallees,arridCnt,arridListedFlag,arridAddedFlag);
        cnt=GetArrayElement(AR_LONG,arridCnt,0);  

        Message("                                       %s  called  all  leaf  funcs\n",name);
        if((cnt==1 && GetArrayElement(AR_LONG,arridCallees,0)==func) || cnt==0)
        {
            Message("\n                                              oh, %s dosnot called any leaf func!\n\n",name);//�Լ��������һ��Ҷ�ڵ㺯��           
        }
        else
        {        
           subcnt=0;//sub_ǰ׺��ʽ�ĺ�������
           for(i=0;i<cnt;i++)
           {
               auto funcaddr,funcname;
               funcaddr=GetArrayElement(AR_LONG,arridCallees,i);
               funcname=Name(funcaddr);                
                       if(strstr(funcname,"sub_")==0)
                       {
                          subcnt++;
                       }
                       if(funcaddr!=func)//�����������
                          Message("                                                %s\n",funcname); 
                    
                           
               
           }
           Message("                                                (sum: %d,  sub_xxx: %d)\n",cnt,subcnt);
        }

        DeleteArray(arridCallees);
        DeleteArray(arridCnt);
        DeleteArray(arridListedFlag);
        DeleteArray(arridAddedFlag);
   }
   else
   {
      Warning("there is no func at current cursor"); 
   }
}





//This function is executed when IDA is started.
static main(void)
{
  auto flagline;
  flagline="-----------------";
  /////////////////////////////////////////////////�ȼ�����/////////////////////////////////////////////////////
  if( AddHotkey("Ctrl+[","GotoCursorFuncStart")!=IDCHK_OK )
     Message("%sAddHotkey Ctrl+[ failed!(goto func start)%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Ctrl+[ succeed!(goto func start)%s\n",flagline,flagline);
     
  if( AddHotkey("Ctrl+]","GotoCursorFuncEnd")!=IDCHK_OK )
     Message("%sAddHotkey Ctrl+] failed!(goto func end)%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Ctrl+] succeed!(goto func end)%s\n",flagline,flagline);

  if( AddHotkey("Shift+Alt+L","ListLeafFuns")!=IDCHK_OK )
     Message("%sAddHotkey Shift+Alt+L(list all unrenamed leaf funcs) failed!%s\n",flagline,flagline);
  else
     Message("%sAddHotkey Shift+Alt+L(list all unrenamed leaf funcs) succeed!%s\n",flagline,flagline);  

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  // uncomment this line to remove full paths in the debugger process options:
  // SetCharPrm(INF_LFLAGS, LFLG_DBG_NOPATH|GetCharPrm(INF_LFLAGS));
}

//-----------------------------------------------------------------------
// Get name of the current processor

static get_processor_name(void)
{
  auto i,procname,chr;

  procname = "";
  for ( i=0; i < 8; i++ )
  {
    chr = GetCharPrm(INF_PROCNAME+i);
    if ( chr == 0 ) break;
    procname = procname + chr;
  }
  return procname;
}

//-----------------------------------------------------------------------
// Get signed extended 16-bit value
static SWord(ea)
{
  auto v;
  v=Word(ea);
  if ( v & 0x8000 ) v = v | 0xFFFF0000;
  return v;
}
