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
static ListCallees(func,arridCallees,arridCnt,arridWalkedFuncs)
{    
      auto name,end,inst,xref,target,calleescnt,WalkedCnt,bLeaf; 	
      name=Name(func);
      end=GetFunctionAttr(func,FUNCATTR_END);
      calleescnt=0;
      bLeaf=1;

      //if(name!=-1)
         //Message("------ListCallees func %s------\n",name);//��ѭ��������һֱruning

      //���ú�������walked����
      WalkedCnt=GetArrayElement(AR_LONG,arridCnt,1);
      SetArrayLong(arridWalkedFuncs,WalkedCnt,func);      
      WalkedCnt++;
      SetArrayLong(arridCnt,1,WalkedCnt);


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
                       //if(name!=-1 && jmpfuncname!=-1)  
                          //Message("--------------------%s jmp %s-------------------\n",name,jmpfuncname);            
                       if( !(GetFunctionFlags(target) & FUNC_LIB) && strstr(jmpfuncname,"j_")!=0  && strstr(jmpfuncname,"xy")!=0 && strstr(jmpfuncname,"XY")!=0 && strstr(jmpfuncname,"Xy")!=0 )
                       {

                           bLeaf=0;
		           ListCallees(target,arridCallees,arridCnt,arridWalkedFuncs); 
                       }
                   }
            }                     
            else if(xref==fl_CN || xref==fl_CF) //�ҵ���һ�����õ�Ŀ�꺯��
	    {
               auto tgtname,i;          
               for(i=0;i<WalkedCnt;i++)
               {
                   if(GetArrayElement(AR_LONG,arridWalkedFuncs,i) == target)
                      break;
               }

               //������õĲ��ǵݹ�����Ҳ������ǰ�Ѿ������ĺ�����Ҳ���ǿ⺯��,Ҳ����xyǰ׺��ͷ���û���������Զ��庯��
               if(i==WalkedCnt)
               {    
                   tgtname=Name(target);       
                   if( !(GetFunctionFlags(target) & FUNC_LIB) && strstr(tgtname,"j_")!=0  && strstr(tgtname,"xy")!=0 && strstr(tgtname,"XY")!=0 && strstr(tgtname,"Xy")!=0 )
                   {
                       calleescnt++;
                       bLeaf=0;
                       //Message("----called %s at %d----\n",name,inst);   
                       ListCallees(target,arridCallees,arridCnt,arridWalkedFuncs);
                   }
               }
               else if(target!=func)//����������ǰ��������Щ����
               {
                   calleescnt++;
                   bLeaf=0;
               }
	    } 
         }
      }
      if(bLeaf)//������������ȫ��������
      {
         auto cnt;
         //Message("%s is a leaf func-------------------------------------------\n",name);
         cnt=GetArrayElement(AR_LONG,arridCnt,0);
         SetArrayLong(arridCallees,cnt,func);
         cnt++;
         SetArrayLong(arridCnt,0,cnt);
      }           
}
//ö�ٹ�괦�������յ��õ����зǿ⡢δ��������Ҷ�ڵ㺯��
static ListLeafFuns()
{
   auto func,name;
   func=GetFunctionAttr(ScreenEA(),FUNCATTR_START);
   name=Name(func);
   if(func!=BADADDR)
   {
        auto arridCallees,arridCnt,arridWalkedFuncs,cnt,i,acnt,subcnt;//callees�����ȫ�ֱ���Ԫ�ظ���
        arridCallees=GetArrayId("xy_callees");
        if(arridCallees!=-1)
        {
           //Message("DeleteArray \"xy_callees\"\n");
           DeleteArray(arridCallees);
        } 
        arridCallees=CreateArray("xy_callees");

        arridCnt=GetArrayId("xy_cnt");
        if(arridCnt!=-1)
        {
           //Message("DeleteArray \"xy_cnt\"\n");
           DeleteArray(arridCnt);
        }
        arridCnt=CreateArray("xy_cnt");
        SetArrayLong(arridCnt,0,0);//cnt1=0,ָҶ�ڵ㺯������


        arridWalkedFuncs=GetArrayId("xy_WalkedFuncs");
        if(arridWalkedFuncs!=-1)
        {
           //Message("DeleteArray \"xy_WalkedFuncs\"\n");
           DeleteArray(arridWalkedFuncs);
        }
        arridWalkedFuncs=CreateArray("xy_WalkedFuncs");
        SetArrayLong(arridCnt,1,0);//cnt2=0,ָ�ѱ����ĺ�������
      
      
	ListCallees(func,arridCallees,arridCnt,arridWalkedFuncs);
        cnt=GetArrayElement(AR_LONG,arridCnt,0);  

        Message("                                       %s  called  all  leaf  funcs\n",name);
        if((cnt==1 && GetArrayElement(AR_LONG,arridCallees,0)==func) || cnt==0)
        {
            Message("\n                                              oh,%s dosnot called any leaf func!\n\n",name);//�Լ��������һ��Ҷ�ڵ㺯��           
        }
        else
        {        
           acnt=0;//ʵ�ʳ�ȥ�ظ���ĺ�������
           subcnt=0;//sub_xxx�����ĸ���
           for(i=0;i<cnt;i++)
           {
               auto funcaddr,funcname;
               funcaddr=GetArrayElement(AR_LONG,arridCallees,i);
               funcname=Name(funcaddr);
               if(i==0)
               {
                  acnt++;
                  if(strstr(funcname,"sub_")==0)
                  {
                      subcnt++;
                  }
                  if(funcaddr!=func)//�����������
                     Message("                                                %s\n",funcname); 
               }                 
               //������������ǰ���ظ��ĺ���
               else
               {
	           auto j;
                   for(j=0;j<i;j++)
                   {
		       if(GetArrayElement(AR_LONG,arridCallees,j) == funcaddr)
                          break;
                   }
                   if(j==i)
                   {
                       acnt++;
                       if(strstr(funcname,"sub_")==0)
                       {
                          subcnt++;
                       }
                       if(funcaddr!=func)//�����������
                          Message("                                                %s\n",funcname); 
                    
                   }                  
                
               }
           }
           Message("                                                (sum: %d,  sub_xxx: %d)\n",acnt,subcnt);
        }

        DeleteArray(arridCallees);
        DeleteArray(arridCnt);
        DeleteArray(arridWalkedFuncs);
   }
   else
   {
      Warning("There is no func at current cursor"); 
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
