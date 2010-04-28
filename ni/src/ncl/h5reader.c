#include "h5data_struct.h"

#define H5_BUFSIZE         (1024 * 1024)

/*-------------------------------------------------------------------------
 * Function:	_NclHDF5check_obj
 *
 * Purpose:	start to check an object
 *
 * Return:	Success: 0
 *		Failure: -1
 *
 * Programmer:	Wei Huang
 *		July 1, 2009
 *
 *-------------------------------------------------------------------------
 */

herr_t _NclHDF5check_obj(const char *filename, NclHDF5group_node_t **HDF5group)
{
    NclHDF5group_node_t *cur_group;
    H5O_info_t oi;              /* Information for object */
    hid_t fid = FAILED;
    herr_t status = FAILED;

    static char root_name[] = "/";

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5check_obj, filename: <%s>, in file: %s, at line: %d\n",
            filename, __FILE__, __LINE__);
#endif

    fid = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);

    if(fid < 0)
    {
        fprintf(stdout, "Unable to open file: <%s>, at line: %d\n", filename, __FILE__, __LINE__);
        H5close();
        return (FAILED);
    }

    /* Retrieve info for object */
    status = H5Oget_info_by_name(fid, root_name, &oi, H5P_DEFAULT);

    if(status == FAILED)
    {
        fprintf(stdout, "Failed to get info for fid: %d, root_name: <%s>, in file: %s, at line: %d\n",
            fid, root_name, __FILE__, __LINE__);
        return FAILED;
    }

    /* Check for group */
    if(H5O_TYPE_GROUP == oi.type)
    {
        hid_t      gid;		/* Object id */
      /*
       *Need to Check if it is external link.
       *if(is_elink())
       *    return SUCCEED;
       */

        /* Get ID for group */
        gid = H5Gopen2(fid, root_name, H5P_DEFAULT);
        if(gid < 0)
        {
            fprintf(stdout, "Unable to open '%s' as group\n", root_name);
            return FAILED;
        }

        cur_group = _NclHDF5allocate_group(fid, filename, root_name, oi.type);

        /* Specified name is a group. Search the complete contents of the group. */
        _NclHDF5recursive_check(fid, root_name,
                                _NclHDF5search_obj, _NclHDF5search_link,
                                cur_group);

        /* Close group */
        H5Gclose(gid);

        H5close();

        *HDF5group = cur_group;
    }
    else
    {
        fprintf(stdout, "<%s> is a non-group object, in file: %s, at line: %d\n",
                root_name, __FILE__, __LINE__);

        (*HDF5group)->num_datasets++;

        /* Specified name is a non-group object -- list that object */
        _NclHDF5search_obj(root_name, &oi, (void *) (*HDF5group), NULL);
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5check_obj, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
    return SUCCEED;
}


/*-------------------------------------------------------------------------
 * Function:	_NclHDF5allocate_group
 *
 * Purpose:	start to check an object
 *
 * Return:	Success: 0
 *		Failure: -1
 *
 * Programmer:	Wei Huang
 *		July 1, 2009
 *
 *-------------------------------------------------------------------------
 */
NclHDF5group_node_t *_NclHDF5allocate_group(hid_t id, char *fname, char *gname, H5O_type_t type)
{
    NclHDF5group_node_t *group_node;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5allocate_group, gname: <%s>, in file: %s, at line: %d\n",
            gname, __FILE__, __LINE__);
#endif

    group_node = calloc(1, sizeof(NclHDF5group_node_t));
    if(group_node == NULL)
    {
        fprintf(stdout, "Failed to allocated memory for group_node.\n");
        fprintf(stdout, "file: %s, line: %d\n", __FILE__, __LINE__);
        return group_node;
    }

    if(fname)
        strcpy(group_node->file, fname);
    strcpy(group_node->name, gname);
    group_node->type = type;
    group_node->id = id;

    group_node->counter = 0;

    group_node->num_attrs = 0;
    group_node->num_links = 0;
    group_node->num_datasets = 0;
    group_node->num_groups = 0;

    group_node->attr_list = NULL;
    group_node->elink_list = NULL;
    group_node->dataset_list = NULL;
    group_node->group_list = NULL;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\tgroup_node->file: <%s>\n", group_node->file);
    fprintf(stdout, "\tgroup_node->name: <%s>\n", group_node->name);
    fprintf(stdout, "\tgroup_node->type: %d\n", group_node->type);
#endif

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5allocate_group, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
    return group_node;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5dim_info
 *
 * Purpose:	get HDF5 dim info.
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 9, 2009
 *
 ***********************************************************************
 */
void _NclHDF5dim_info(NclHDF5dim_list_t **NclHDF5dim_list,
                      NclHDF5attr_list_t *NclHDF5attr_list)
{
    NclHDF5attr_list_t  *curHDF5attr_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5dim_info, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    curHDF5attr_list = NclHDF5attr_list;

    while(curHDF5attr_list)
    {
        NclHDF5attr_node_t *attr_node = curHDF5attr_list->attr_node;

#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tname: <%s>\n", attr_node->name);
        fprintf(stdout, "\ttype_name: <%s>\n", attr_node->type_name);
        fprintf(stdout, "\tndims: %d\n", attr_node->ndims);
#endif

        if((0 == strcmp(attr_node->name, "Dimensions")) &&
           (0 == strcmp(attr_node->type_name, "string")))
        {
            int i = 0;
            int na = 1;
            char *tmpString = NULL;
            char *result = NULL;
            NclHDF5dim_list_t *cur_dim_list;

            tmpString = strdup((char *)attr_node->value);

            result = strtok(tmpString, " ");
            i = 0;
            while(result != NULL)
            {
                cur_dim_list = calloc(1, sizeof(NclHDF5dim_list_t));

                strcpy(cur_dim_list->name, result);

                cur_dim_list->next = *NclHDF5dim_list;
                *NclHDF5dim_list = cur_dim_list;

                result = strtok(NULL, " ");
                if(i < attr_node->ndims)
                    na *= attr_node->dims[i];
                i++;
                if(i >= na)
                    break;
            }

            free(tmpString);
#if DEBUG_NCL_HDF5
            fprintf(stdout, "Leaving _NclHDF5dim_info, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
            return;
        }

        curHDF5attr_list = curHDF5attr_list->next;
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5dim_info, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5var_list
 *
 * Purpose:	get variable list (name, dims, dimsname)
 *
 * Return:	pointer to NclHDF5var_list_t
 *
 * Programmer:	Wei Huang
 * Created:	July 9, 2009
 *
 ***********************************************************************
 */
void _NclHDF5var_list(NclHDF5var_list_t **var_list, NclHDF5group_node_t *HDF5group)
{
    NclHDF5var_list_t *cur_list = NULL;

    NclHDF5dataset_list_t *dataset_list;
    NclHDF5group_list_t   *group_list;

    int i = 0;

    if(!HDF5group)
        return;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5var_list, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    dataset_list = HDF5group->dataset_list;

    while(dataset_list)
    {
        NclHDF5dataset_node_t *dataset_node = dataset_list->dataset_node;
        NclHDF5dim_list_t *NclHDF5dim_list = NULL;
        NclHDF5dim_list_t *curDimList;

        if(dataset_node->attr_list)
            _NclHDF5dim_info(&NclHDF5dim_list, dataset_node->attr_list);

        cur_list = calloc(1, sizeof(NclHDF5var_list_t));
        if(!cur_list)
        {
            fprintf(stdout, "UNABLE TO ALLOCATE MEMORY for cur_list, in file: %s, line: %d\n",
                    __FILE__, __LINE__);
            return;
        }

        strcpy(cur_list->name, dataset_node->name);

        cur_list->next = *var_list;
        *var_list = cur_list;
        cur_list->ndims = dataset_node->ndims;

#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tname=<%s>\n", dataset_node->name);
        fprintf(stdout, "\tgroup_name=<%s>\n", dataset_node->group_name);
        fprintf(stdout, "\tshort_name=<%s>\n", dataset_node->short_name);
        fprintf(stdout, "\tcur_list->ndims=%d\n", cur_list->ndims);
#endif
      
        curDimList = NclHDF5dim_list;
        i = 0;
        while(curDimList)
        {
            curDimList->size = dataset_node->dims[i];
            cur_list->dims[i] = dataset_node->dims[i];

            strcpy(dataset_node->dim_name[i], curDimList->name);
            strcpy(cur_list->dim_name[i], curDimList->name);
#if DEBUG_NCL_HDF5
            fprintf(stdout, "\tdims[%d] name = <%s>\n", i, curDimList->name);
            fprintf(stdout, "\tdims[%d] size = %d\n", i, curDimList->size);
            fprintf(stdout, "\tcur_list->dim_name[%d] = <%s>\n", i, cur_list->dim_name[i]);
            fprintf(stdout, "\tcur_list->dims[%d]     = %d\n", i, cur_list->dims[i]);
#endif

            curDimList = curDimList->next;
            i++;
            if(i >= dataset_node->ndims)
                break;
        }

        _NclFree_HDF5dim_list(NclHDF5dim_list);
        dataset_list = dataset_list->next;
    }

    group_list = HDF5group->group_list;
    while(group_list)
    {
        _NclHDF5var_list(var_list, group_list->group_node);
        
        group_list = group_list->next;
    }
#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5var_list, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclFree_HDF5dim_list
 *
 * Purpose:	Free NclHDF5dim_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 9, 2009
 *
 ***********************************************************************
 */
void _NclFree_HDF5dim_list(NclHDF5dim_list_t *NclHDF5dim_list)
{
    NclHDF5dim_list_t *cur_dim_list;

    cur_dim_list = NclHDF5dim_list;

    while(cur_dim_list)
    {
        NclHDF5dim_list = cur_dim_list->next;
        cur_dim_list->next = NULL;

        free(cur_dim_list);

        cur_dim_list = NclHDF5dim_list;
    }
}


/*
 ***********************************************************************
 * Function:	_NclFree_HDF5var_list
 *
 * Purpose:	Free HDF5var_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 9, 2009
 *
 ***********************************************************************
 */
void _NclFree_HDF5var_list(NclHDF5var_list_t *HDF5var_list)
{
    NclHDF5var_list_t *cur_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclFree_HDF5var_list, in file: %s, at line: %d\n",
            __FILE__, __LINE__);
#endif

    cur_list = HDF5var_list;

    while(cur_list)
    {
        HDF5var_list = cur_list->next;
        cur_list->next = NULL;

        free(cur_list);

        cur_list = HDF5var_list;
    }
#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclFree_HDF5var_list, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5print_dataset
 *
 * Purpose:	print the dataset
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 5, 2009
 *
 ***********************************************************************
 */

void _NclHDF5print_dataset(hid_t dset, char *type_name)
{
    H5S_class_t space_type;
    hid_t       f_space;
    hid_t       p_type;
    herr_t      status = FAILED;
    hid_t       f_type = H5Dget_type(dset);
    size_t      size = H5Tget_size(f_type);
    
    fprintf(stdout, "\nEnter _NclHDF5print_dataset, dset=%d, type_name: <%s>, at file: %s, line: %d\n",
            dset, type_name, __FILE__, __LINE__);

    f_type = H5Dget_type(dset);

    p_type = H5Tcopy(f_type);

    /* Check the data space */
    f_space = H5Dget_space(dset);

    space_type = H5Sget_simple_extent_type(f_space);

#if 0
    fprintf(stdout, "\tf_type = %d\n", f_type);
    fprintf(stdout, "\tp_type = %d\n", p_type);
    fprintf(stdout, "\tf_space = %d\n", f_space);
    fprintf(stdout, "\tspace_type = %d\n", space_type);
    fprintf(stdout, "\tH5S_SIMPLE = %d\n", H5S_SIMPLE);
    fprintf(stdout, "\tH5S_SCALAR = %d\n", H5S_SCALAR);
#endif

    /* Print the data */
    switch(space_type)
    {
        case H5S_SCALAR:
        case H5S_SIMPLE:
            status = _NclHDF5print_simple_dataset(dset, p_type, type_name);
            break;
        default:
            status = SUCCEED;
    }

    /* Close the dataspace */
    H5Sclose(f_space);

    fprintf(stdout, "Leave _NclHDF5print_dataset at file: %s, line: %d\n\n", __FILE__, __LINE__);
}


/*
 ***********************************************************************
 * Function:	_NclHDF5print_simple_dataset
 *
 * Purpose:	print the simple dataset
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 5, 2009
 *
 ***********************************************************************
 */

int _NclHDF5print_simple_dataset(hid_t dset, hid_t p_type, char *type_name)
{
    hid_t               f_space;                  /* file data space */
    hsize_t             elmtno;                   /* counter  */
    size_t              i;                        /* counter  */
    int                 carry;                    /* counter carry value */
    hsize_t             zero[8];                  /* vector of zeros */
    unsigned int        flags;                    /* buffer extent flags */
    hsize_t             total_size[H5S_MAX_RANK]; /* total size of dataset*/

    /* Print info */
    size_t              p_type_nbytes;            /* size of memory type */
    hsize_t             p_nelmts;                 /* total selected elmts */

    /* Stripmine info */
    hsize_t             sm_size[H5S_MAX_RANK];    /* stripmine size */
    hsize_t             sm_nbytes;                /* bytes per stripmine */
    hsize_t             sm_nelmts;                /* elements per stripmine*/
    unsigned char       *sm_buf = NULL;           /* buffer for raw data */
    hid_t               sm_space;                 /* stripmine data space */

    /* Hyperslab info */
    hsize_t            hs_offset[H5S_MAX_RANK];   /* starting offset */
    hsize_t            hs_size[H5S_MAX_RANK];     /* size this pass */
    hsize_t            hs_nelmts;                 /* elements in request */

    /* VL data special information */
    unsigned int       vl_data = FALSE;               /* contains VL datatypes */

    int                ndims;
    hsize_t            p_min_idx[H5S_MAX_RANK];
    hsize_t            p_max_idx[H5S_MAX_RANK];
    hsize_t            size_last_dim;

    fprintf(stdout, "\nEnter _NclHDF5print_simple_dataset, dset=%d, type_name: <%s>, at file: %s, line: %d\n",
            dset, type_name, __FILE__, __LINE__);

    f_space = H5Dget_space(dset);

    if (f_space == FAILED)
        return FAILED;

    ndims = H5Sget_simple_extent_ndims(f_space);

    if (ndims > H5S_MAX_RANK)
    {
        H5Sclose(f_space);
        return FAILED;
    }

    /* Assume entire data space to be printed */
    if (ndims > 0)
    {
        for (i = 0; i < (size_t)ndims; i++)
            p_min_idx[i] = 0;
    }

    H5Sget_simple_extent_dims(f_space, total_size, NULL);

    /* calculate the number of elements we're going to print */
    p_nelmts = 1;

    if (ndims > 0)
    {
        size_last_dim = (total_size[ndims - 1]);
    }
    else
    {
        size_last_dim = 0;
    }

    /* Check if we have VL data in the dataset's datatype */
    if (H5Tdetect_class(p_type, H5T_VLEN) == TRUE)
        vl_data = TRUE;

    /*
     * Determine the strip mine size and allocate a buffer. The strip mine is
     * a hyperslab whose size is manageable.
     */
    sm_nbytes = p_type_nbytes = H5Tget_size(p_type);

    if (ndims > 0)
    {
        for (i = ndims; i > 0; --i)
        {
            sm_size[i - 1] = H5_BUFSIZE / sm_nbytes;
            if( sm_size[i - 1] > total_size[i - 1])
                sm_size[i - 1] = total_size[i - 1];
            sm_nbytes *= sm_size[i - 1];
            assert(sm_nbytes > 0);
        }
    }

    assert(sm_nbytes == (hsize_t)((size_t)sm_nbytes)); /*check for overflow*/
    sm_buf = malloc((size_t)sm_nbytes);

    sm_nelmts = sm_nbytes / p_type_nbytes;

    sm_space = H5Screate_simple(1, &sm_nelmts, NULL);

#if 0
    fprintf(stdout, "\tsm_nelmts = %d\n", sm_nelmts);
    fprintf(stdout, "\tsm_nbytes = %d\n", sm_nbytes);
    fprintf(stdout, "\tp_type_nbytes = %d\n", p_type_nbytes);
    fprintf(stdout, "\tsm_space = %d\n", sm_space);
#endif

    /* The stripmine loop */
    memset(hs_offset, 0, sizeof hs_offset);
    memset(zero, 0, sizeof zero);

    for (elmtno = 0; elmtno < p_nelmts; elmtno += hs_nelmts)
    {
        /* Calculate the hyperslab size */
        if (ndims > 0)
        {
            hs_nelmts = 1;
            for (i = 0; i < ndims; i++)
            {
                hs_size[i] = total_size[i] - hs_offset[i];
                if (hs_size[i] > sm_size[i])
                    hs_size[i] = sm_size[i];
                p_max_idx[i] = p_min_idx[i] + hs_size[i];
                hs_nelmts *= hs_size[i];
#if 0
                fprintf(stdout, "\ttotal_size[%d]=%d\n", i, total_size[i]);
                fprintf(stdout, "\ths_offset[%d]=%d\n", i, hs_offset[i]);
                fprintf(stdout, "\tsm_size[%d]=%d\n", i, sm_size[i]);
                fprintf(stdout, "\ths_size[%d]=%d\n", i, hs_size[i]);
                fprintf(stdout, "\tp_min_idx[%d]=%d\n", i, p_min_idx[i]);
                fprintf(stdout, "\tp_max_idx[%d]=%d\n", i, p_max_idx[i]);
                fprintf(stdout, "\ths_nelmts = %d\n", hs_nelmts);
#endif
            }

            H5Sselect_hyperslab(f_space, H5S_SELECT_SET, hs_offset, NULL,
                                hs_size, NULL);
            H5Sselect_hyperslab(sm_space, H5S_SELECT_SET, zero, NULL,
                                &hs_nelmts, NULL);
        }
        else
        {
            H5Sselect_all(f_space);
            H5Sselect_all(sm_space);
            hs_nelmts = 1;
        }

        /* Read the data */
        if (H5Dread(dset, p_type, sm_space, f_space, H5P_DEFAULT, sm_buf) < 0)
        {
            H5Sclose(f_space);
            H5Sclose(sm_space);
            free(sm_buf);
            return FAILED;
        }

        _NclHDF5Print_data_value(sm_buf, ndims, sm_size, type_name);
    }

    H5Sclose(sm_space);
    H5Sclose(f_space);

    free(sm_buf);

    fprintf(stdout, "Leave _NclHDF5print_simple_dataset at file: %s, line: %d\n\n", __FILE__, __LINE__);

    return SUCCEED;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_data_with_name
 *
 * Purpose:	Read HDF5 data based on dataset-name
 *
 * Return:	pointer to NclHDF5data_t
 *
 * Programmer:	Wei Huang
 * Created:	July 9, 2009
 *
 ***********************************************************************
 */
NclHDF5data_t *_NclHDF5get_data_with_name(hid_t fid, char *dataset_name, NclHDF5group_node_t *HDF5group)
{
    NclHDF5data_t *NclHDF5data = NULL;
    unsigned char *uc_value;

    if(HDF5group)
    {
        NclHDF5group_list_t   *curHDF5group_list;
        NclHDF5attr_list_t    *curHDF5attr_list;
        NclHDF5dataset_list_t *curHDF5dataset_list;
        NclHDF5dataset_node_t *dataset;
        unsigned long size = 1;

        int i = 0;
        int nd = 0;
        int ng = 0;

        char *dot_ptr;
        char the_name[1024];
        char component[1024];

        strcpy(the_name, dataset_name);
        dot_ptr = strchr(the_name, '.');
        if(dot_ptr)
        {
            strcpy(component, dot_ptr+1);
            dot_ptr[0] = '\0';
        }

        curHDF5dataset_list = HDF5group->dataset_list;

        while(curHDF5dataset_list)
        {
            nd++;
            dataset = curHDF5dataset_list->dataset_node;
            if((0 == strcmp(the_name, dataset->name)) ||
               (0 == strcmp(the_name, dataset->short_name)))
            {
                NclHDF5data = _NclHDF5allocate_data(dataset->id);

                strcpy(NclHDF5data->name, dataset->name);
                strcpy(NclHDF5data->type, dataset->type_name);

                NclHDF5data->ndims = dataset->ndims;

#if 0
                fprintf(stdout, "\n\n\tFind data:\n");
                fprintf(stdout, "\tNclHDF5data->id: %d\n", NclHDF5data->id);

                fprintf(stdout, "\ttype: <%s>\n", NclHDF5data->type);
                fprintf(stdout, "\tname: <%s>\n", NclHDF5data->name);
                fprintf(stdout, "\tgroup_name: <%s>\n", dataset->group_name);
                fprintf(stdout, "\tshort_name: <%s>\n", dataset->short_name);
#endif

                if(0 == strcmp(NclHDF5data->type, "compound"))
                {
                  /*
                   *size = (unsigned long) dataset->compound->size;
                   */
                    for(i = 0; i < dataset->compound->nom; i++)
                    {
                        if(0 == strcmp(component, dataset->compound->member[i].name))
                        {
                            size = NclHDF5sizeof(dataset->compound->member[i].type);
                            break;
                        }
                    }
                }
                else
                {
                    size = NclHDF5sizeof(NclHDF5data->type);
                    if(! size)
                    {
                        fprintf(stdout, "\tDon't know the type: <%s> in file: %s, line: %d\n",
                                NclHDF5data->type, __FILE__, __LINE__);
                    }
                }

                NclHDF5data->nbytes = size;
                if(NclHDF5data->ndims > 0)
                {
                    for(i=0; i<NclHDF5data->ndims; i++)
                    {
                        NclHDF5data->dims[i] = dataset->dims[i];
                        NclHDF5data->nbytes *= dataset->dims[i];
                    }
                }

#if 0
                curHDF5attr_list = dataset->attr_list;
                while(curHDF5attr_list)
                {
                    if((0 == strcmp(curHDF5attr_list->attr_node->name, "Dimensions")) &&
                       (0 == strcmp(curHDF5attr_list->attr_node->type_name, "string")))
                    {
                        char *tmpString = NULL;
                        char *result = NULL;
                        tmpString = strdup((char *)curHDF5attr_list->attr_node->value);
                        fprintf(stdout, "\tdim_string: <%s>\n", tmpString);
   
                        result = strtok(tmpString, " ");
                        i = 0;
                        while(result != NULL)
                        {
                            fprintf(stdout, "\tdim_name[%d]: <%s>\n", i, result);
                            strcpy(NclHDF5data->dim_name[i], result);
                            fprintf(stdout, "\tdim_name[%d]: <%s>\n", i, NclHDF5data->dim_name[i]);
                            result = strtok(NULL, " ");
                            i++;
                            if(i >= NclHDF5data->ndims)
                                break;
                        }

                        free(tmpString);
                        break;
                    }

                    curHDF5attr_list = curHDF5attr_list->next;
                }

                for(i = 0; i < NclHDF5data->ndims; i++)
                {
                    fprintf(stdout, "\tdims[%d] = %d\n", i, NclHDF5data->dims[i]);
                    fprintf(stdout, "\tdim_name[%d]: <%s>\n", i, NclHDF5data->dim_name[i]);
                }
#endif

                uc_value = _NclHDF5get_native_dataset(fid, dataset->name,
                                                      NclHDF5data->type,
                                                      dataset->compound,
                                                      component);
                NclHDF5data->value = (void *) uc_value;
#if 0
                _NclHDF5Print_data_value(NclHDF5data->value, NclHDF5data->ndims,
                                         NclHDF5data->dims, NclHDF5data->type);
#endif

              /*
               *uc_value = _NclHDF5get_dataset(fid, dataset->name, NclHDF5data->id, NclHDF5data->type);
               *NclHDF5data->value = (void *) uc_value;

               *_NclHDF5Print_data_value(NclHDF5data->value, NclHDF5data->ndims,
               *                         NclHDF5data->dims, NclHDF5data->type);
               */

                return NclHDF5data;
            }

            curHDF5dataset_list = curHDF5dataset_list->next;
        }

        curHDF5group_list = HDF5group->group_list;

        while(curHDF5group_list)
        {
            ng++;

            if(curHDF5group_list->group_node)
            {
                NclHDF5data = _NclHDF5get_data_with_name(fid, dataset_name, curHDF5group_list->group_node);
                if(NclHDF5data)
                    return NclHDF5data;
            }
            else
            {
                fprintf(stdout, "curHDF5group_list->group_node is NULL.\n");
            }

            curHDF5group_list = curHDF5group_list->next;
        }
    }

    return NclHDF5data;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_data_with_id
 *
 * Purpose:	Read HDF5 data based on ID
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 8, 2009
 *
 ***********************************************************************
 */
NclHDF5data_t *_NclHDF5get_data_with_id(hid_t fid, hid_t did, NclHDF5group_node_t *HDF5group)
{
    NclHDF5data_t *NclHDF5data = NULL;
    unsigned char *uc_value;
    unsigned long size = 1;

    if(HDF5group != NULL)
    {
        NclHDF5attr_list_t    *curHDF5attr_list;
        NclHDF5attr_node_t    *attr_node;
        NclHDF5group_list_t   *curHDF5group_list;
        NclHDF5dataset_list_t *curHDF5dataset_list;
        NclHDF5dataset_node_t *dataset;

        int i = 0;
        int nd = 0;
        int ng = 0;

        curHDF5dataset_list = HDF5group->dataset_list;

        while(curHDF5dataset_list)
        {
            nd++;
            dataset = curHDF5dataset_list->dataset_node;
            if(did == dataset->id)
            {
                NclHDF5data = _NclHDF5allocate_data(did);

                strcpy(NclHDF5data->name, dataset->name);
                strcpy(NclHDF5data->type, dataset->type_name);

                if(0 == strcmp(NclHDF5data->type, "compound"))
                {
                    size = (unsigned long) dataset->compound->size;
                }
                else
                {
                    size = NclHDF5sizeof(NclHDF5data->type);
                    if(! size)
                    {
                        fprintf(stdout, "\tDon't know the type: <%s> in file: %s, line: %d\n",
                                NclHDF5data->type, __FILE__, __LINE__);
                    }
                }

                NclHDF5data->ndims = dataset->ndims;
                NclHDF5data->nbytes = size;

                if(NclHDF5data->ndims > 0)
                {
                    for(i=0; i<NclHDF5data->ndims; i++)
                    {
                        NclHDF5data->dims[i] = dataset->dims[i];
                        NclHDF5data->nbytes *= dataset->dims[i];
                    }
                }

                curHDF5attr_list = dataset->attr_list;
                while(curHDF5attr_list)
                {
                    attr_node = curHDF5attr_list->attr_node;
                    if((0 == strcmp(attr_node->name, "Dimensions")) &&
                       (0 == strcmp(attr_node->type_name, "string")))
                    {
                        char *tmpString = NULL;
                        char *result = NULL;
                        tmpString = strdup((char *)attr_node->value);
   
                        result = strtok(tmpString, " ");
                        i = 0;
                        while(result != NULL)
                        {
                            strcpy(NclHDF5data->dim_name[i], result);
                            result = strtok(NULL, " ");
                            i++;
                            if(i >= NclHDF5data->ndims)
                                break;
                        }

                        free(tmpString);
                        break;
                    }

                    curHDF5attr_list = curHDF5attr_list->next;
                }

              /*
                if(strcmp("integer", NclHDF5data->type) == 0)
                {
                    uc_value = _NclHDF5get_native_dataset(fid, dataset->short_name,
                                                          NclHDF5data->id, NclHDF5data->type,
                                                          dataset->compound,
                                                          component);
                    NclHDF5data->value = (void *) uc_value;
                    _NclHDF5Print_data_value(NclHDF5data->value, NclHDF5data->ndims,
                                             NclHDF5data->dims, NclHDF5data->type);
                }
                uc_value = _NclHDF5get_dataset(fid, dataset->short_name, NclHDF5data->id, NclHDF5data->type);
               */

                uc_value = _NclHDF5get_dataset(fid, dataset->name, NclHDF5data->id, NclHDF5data->type);
                NclHDF5data->value = (void *) uc_value;

#if 0
                _NclHDF5Print_data_value(NclHDF5data->value, NclHDF5data->ndims,
                                         NclHDF5data->dims, NclHDF5data->type);
#endif

                return NclHDF5data;
            }

            curHDF5dataset_list = curHDF5dataset_list->next;
        }

        curHDF5group_list = HDF5group->group_list;

        while(curHDF5group_list)
        {
            ng++;

            if(curHDF5group_list->group_node)
            {
                NclHDF5data = _NclHDF5get_data_with_id(fid, did, curHDF5group_list->group_node);
                if(NclHDF5data)
                    return NclHDF5data;
            }
            else
            {
                fprintf(stdout, "curHDF5group_list->group_node is NULL.\n");
            }

            curHDF5group_list = curHDF5group_list->next;
        }
    }

    return NclHDF5data;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5allocate_data
 *
 * Purpose:	allocate memory for NclHDF5data
 *
 * Return:	point to NclHDF5data_t
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
NclHDF5data_t *_NclHDF5allocate_data(hid_t id)
{
    NclHDF5data_t *NclHDF5data;
    int i = 0;

    NclHDF5data = calloc(1, sizeof(NclHDF5data_t));

    NclHDF5data->id = id;

    NclHDF5data->ndims = 0;
    NclHDF5data->nbytes = 1;

    NclHDF5data->value = NULL;

    return NclHDF5data;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5free_data
 *
 * Purpose:	Free NclHDF5data
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclHDF5free_data(NclHDF5data_t *NclHDF5data)
{
    int i = 0;

    if(NclHDF5data)
    {
        if(NclHDF5data->value)
            free(NclHDF5data->value);
        free(NclHDF5data);
    }
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_dataset
 *
 * Purpose:	get the dataset
 *
 * Return:	pointer to unsigned char
 *
 * Programmer:	Wei Huang
 * Created:	July 8, 2009
 *
 ***********************************************************************
 */

unsigned char *_NclHDF5get_dataset(hid_t fid, char *dataset_name, hid_t dset, char *type_name)
{
    hid_t       did;
    H5S_class_t space_type;
    hid_t       f_space;
    hid_t       p_type;
    herr_t      status = FAILED;
    hid_t       f_type = H5Dget_type(dset);
    size_t      size = H5Tget_size(f_type);

    unsigned char *value;

    fprintf(stdout, "\n\n\nhit _NclHDF5get_dataset. file: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stdout, "\tdataset_name: <%s>\n", dataset_name);
    fprintf(stdout, "\ttype_name: <%s>\n", type_name);

    did = H5Dopen2(fid, dataset_name, H5P_DEFAULT);

    /*
     * Get datatype and dataspace handles and then query
     * dataset class, order, size, rank and dimensions.
     */
    f_type = H5Dget_type(did);

    p_type = H5Tcopy(f_type);

    /* Check the data space */
    f_space = H5Dget_space(did);

    space_type = H5Sget_simple_extent_type(f_space);

#if 0
    fprintf(stdout, "\tf_type = %d\n", f_type);
    fprintf(stdout, "\tp_type = %d\n", p_type);
    fprintf(stdout, "\tf_space = %d\n", f_space);
    fprintf(stdout, "\tspace_type = %d\n", space_type);
    fprintf(stdout, "\tH5S_SIMPLE = %d\n", H5S_SIMPLE);
    fprintf(stdout, "\tH5S_SCALAR = %d\n", H5S_SCALAR);
#endif

    /* Print the data */
    switch(space_type)
    {
        case H5S_SCALAR:
        case H5S_SIMPLE:
            value = _NclHDF5get_simple_dataset(did, p_type, type_name);
            break;
        default:
            value = NULL;
    }

    /* Close the dataspace */
    H5Sclose(f_space);

    return value;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_simple_dataset
 *
 * Purpose:	get the simple dataset
 *
 * Return:	pointer to unsigned char
 *
 * Programmer:	Wei Huang
 * Created:	July 8, 2009
 *
 ***********************************************************************
 */

unsigned char *_NclHDF5get_simple_dataset(hid_t dset, hid_t p_type, char *type_name)
{
    hid_t               f_space;                  /* file data space */
    hsize_t             elmtno;                   /* counter  */
    size_t              i;                        /* counter  */
    int                 carry;                    /* counter carry value */
    hsize_t             zero[8];                  /* vector of zeros */
    unsigned int        flags;                    /* buffer extent flags */
    hsize_t             total_size[H5S_MAX_RANK]; /* total size of dataset*/

    /* Print info */
    size_t              p_type_nbytes;            /* size of memory type */
    hsize_t             p_nelmts;                 /* total selected elmts */

    /* Stripmine info */
    hsize_t             sm_size[H5S_MAX_RANK];    /* stripmine size */
    hsize_t             sm_nbytes;                /* bytes per stripmine */
    hsize_t             sm_nelmts;                /* elements per stripmine*/
    unsigned char       *sm_buf = NULL;           /* buffer for raw data */
    hid_t               sm_space;                 /* stripmine data space */

    /* Hyperslab info */
    hsize_t            hs_offset[H5S_MAX_RANK];   /* starting offset */
    hsize_t            hs_size[H5S_MAX_RANK];     /* size this pass */
    hsize_t            hs_nelmts;                 /* elements in request */

    /* VL data special information */
    unsigned int       vl_data = FALSE;               /* contains VL datatypes */

    int                ndims;
    hsize_t            p_min_idx[H5S_MAX_RANK];
    hsize_t            p_max_idx[H5S_MAX_RANK];
    hsize_t            size_last_dim;

    f_space = H5Dget_space(dset);

    if (f_space == FAILED)
        return NULL;

    ndims = H5Sget_simple_extent_ndims(f_space);

    if (ndims > H5S_MAX_RANK)
    {
        H5Sclose(f_space);
        return NULL;
    }

    /* Assume entire data space to be printed */
    if (ndims > 0)
    {
        for (i = 0; i < (size_t)ndims; i++)
            p_min_idx[i] = 0;
    }

    H5Sget_simple_extent_dims(f_space, total_size, NULL);

    /* calculate the number of elements we're going to print */
    p_nelmts = 1;

    if (ndims > 0)
    {
        size_last_dim = (total_size[ndims - 1]);
    }
    else
    {
        size_last_dim = 0;
    }

    /* Check if we have VL data in the dataset's datatype */
    if (H5Tdetect_class(p_type, H5T_VLEN) == TRUE)
        vl_data = TRUE;

    /*
     * Determine the strip mine size and allocate a buffer. The strip mine is
     * a hyperslab whose size is manageable.
     */
    sm_nbytes = p_type_nbytes = H5Tget_size(p_type);

    if (ndims > 0)
    {
        for (i = ndims; i > 0; --i)
        {
            sm_size[i - 1] = H5_BUFSIZE / sm_nbytes;
            if( sm_size[i - 1] > total_size[i - 1])
                sm_size[i - 1] = total_size[i - 1];
            sm_nbytes *= sm_size[i - 1];
            assert(sm_nbytes > 0);
        }
    }

    assert(sm_nbytes == (hsize_t)((size_t)sm_nbytes)); /*check for overflow*/
    sm_buf = malloc((size_t)sm_nbytes);

    sm_nelmts = sm_nbytes / p_type_nbytes;

    sm_space = H5Screate_simple(1, &sm_nelmts, NULL);

#if 0
    fprintf(stdout, "\tsm_nelmts = %d\n", sm_nelmts);
    fprintf(stdout, "\tsm_nbytes = %d\n", sm_nbytes);
    fprintf(stdout, "\tp_type_nbytes = %d\n", p_type_nbytes);
    fprintf(stdout, "\tsm_space = %d\n", sm_space);
#endif

    /* The stripmine loop */
    memset(hs_offset, 0, sizeof hs_offset);
    memset(zero, 0, sizeof zero);

    for (elmtno = 0; elmtno < p_nelmts; elmtno += hs_nelmts)
    {
        /* Calculate the hyperslab size */
        if (ndims > 0)
        {
            hs_nelmts = 1;
            for (i = 0; i < ndims; i++)
            {
                hs_size[i] = total_size[i] - hs_offset[i];
                if (hs_size[i] > sm_size[i])
                    hs_size[i] = sm_size[i];
                p_max_idx[i] = p_min_idx[i] + hs_size[i];
                hs_nelmts *= hs_size[i];
#if 0
                fprintf(stdout, "\ttotal_size[%d]=%d\n", i, total_size[i]);
                fprintf(stdout, "\ths_offset[%d]=%d\n", i, hs_offset[i]);
                fprintf(stdout, "\tsm_size[%d]=%d\n", i, sm_size[i]);
                fprintf(stdout, "\ths_size[%d]=%d\n", i, hs_size[i]);
                fprintf(stdout, "\tp_min_idx[%d]=%d\n", i, p_min_idx[i]);
                fprintf(stdout, "\tp_max_idx[%d]=%d\n", i, p_max_idx[i]);
                fprintf(stdout, "\ths_nelmts = %d\n", hs_nelmts);
#endif
            }

            H5Sselect_hyperslab(f_space, H5S_SELECT_SET, hs_offset, NULL,
                                hs_size, NULL);
            H5Sselect_hyperslab(sm_space, H5S_SELECT_SET, zero, NULL,
                                &hs_nelmts, NULL);
        }
        else
        {
            H5Sselect_all(f_space);
            H5Sselect_all(sm_space);
            hs_nelmts = 1;
        }

        /* Read the data */
        if (H5Dread(dset, p_type, sm_space, f_space, H5P_DEFAULT, sm_buf) < 0)
        {
            H5Sclose(f_space);
            H5Sclose(sm_space);
            free(sm_buf);
            return NULL;
        }

      /*
        _NclHDF5Print_data_value(sm_buf, ndims, sm_size, type_name);
       */
    }

    H5Sclose(sm_space);
    H5Sclose(f_space);

    return sm_buf;
}


hid_t Ncl2HDF5type(const char *type)
{
    hid_t h5type = -1;
    if(strcmp("integer", type) == 0)
    {
        h5type = H5T_NATIVE_INT;
    }
    else if(strcmp("int", type) == 0)
    {
        h5type = H5T_NATIVE_INT;
    }
    else if(strcmp("uint", type) == 0)
    {
        h5type = H5T_NATIVE_UINT;
    }
    else if(strcmp("long", type) == 0)
    {
        h5type = H5T_NATIVE_LONG;
    }
    else if(strcmp("ulong", type) == 0)
    {
        h5type = H5T_NATIVE_ULONG;
    }
    else if(strcmp("int64", type) == 0)
    {
        h5type = H5T_NATIVE_LLONG;
    }
    else if(strcmp("uint64", type) == 0)
    {
        h5type = H5T_NATIVE_ULLONG;
    }
    else if(strcmp("short", type) == 0)
    {
        h5type = H5T_NATIVE_SHORT;
    }
    else if(strcmp("ushort", type) == 0)
    {
        h5type = H5T_NATIVE_USHORT;
    }
    else if(strcmp("byte", type) == 0)
    {
        h5type = H5T_NATIVE_CHAR;
    }
    else if(strcmp("char", type) == 0)
    {
        h5type = H5T_NATIVE_CHAR;
    }
    else if(strcmp("float", type) == 0)
    {
        h5type = H5T_NATIVE_FLOAT;
    }
    else if(strcmp("double", type) == 0)
    {
        h5type = H5T_NATIVE_DOUBLE;
    }
    else if(strcmp("string", type) == 0)
    {
        h5type = H5T_NATIVE_CHAR;
    }
    else if(strcmp("compound", type) == 0)
    {
        h5type = H5T_COMPOUND;
    }
    else
    {
        fprintf(stdout, "\nUNKOWN TYPE: <%s>. file: %s, line: %d\n", type, __FILE__, __LINE__);
    }

    return h5type;
}


unsigned long NclHDF5sizeof(const char *type)
{
    unsigned long size = 0;

    if(strcmp("integer", type) == 0)
    {
        size = sizeof(int);
    }
    else if(strcmp("int", type) == 0)
    {
        size = sizeof(int);
    }
    else if(strcmp("uint", type) == 0)
    {
        size = sizeof(unsigned int);
    }
    else if(strcmp("long", type) == 0)
    {
        size = sizeof(long);
    }
    else if(strcmp("ulong", type) == 0)
    {
        size = sizeof(unsigned long);
    }
    else if(strcmp("int64", type) == 0)
    {
        size = sizeof(long long);
    }
    else if(strcmp("uint64", type) == 0)
    {
        size = sizeof(unsigned long long);
    }
    else if(strcmp("short", type) == 0)
    {
        size = sizeof(short);
    }
    else if(strcmp("ushort", type) == 0)
    {
        size = sizeof(unsigned short);
    }
    else if(strcmp("byte", type) == 0)
    {
        size = sizeof(unsigned char);
    }
    else if(strcmp("float", type) == 0)
    {
        size = sizeof(float);
    }
    else if(strcmp("double", type) == 0)
    {
        size = sizeof(double);
    }
    else if(strcmp("char", type) == 0)
    {
        size = sizeof(char);
    }
    else if(strcmp("string", type) == 0)
    {
        size = sizeof(char);
    }
    else
    {
        fprintf(stdout, "\nUNKOWN TYPE: <%s>. file: %s, line: %d\n", type, __FILE__, __LINE__);
    }

    return size;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_native_dataset
 *
 * Purpose:	get the dataset
 *
 * Return:	pointer to unsigned char
 *
 * Programmer:	Wei Huang
 * Created:	July 5, 2009
 *
 ***********************************************************************
 */

unsigned char *_NclHDF5get_native_dataset(hid_t fid, char *dataset_name, char *type,
                                          NclHDF5compound_t *compound, const char *component)
{
    hid_t       did;
    hid_t       h5type;
    hid_t       datatype, dataspace;
    hid_t       memspace;
    H5T_class_t t_class;               /* data type class */
    H5T_order_t order;                 /* data order */
    size_t      datasize;              /* size of the data element stored in file */
    herr_t      status;

    hsize_t     *count;              /* size of the hyperslab in the file */
    hsize_t     *offset;             /* hyperslab offset in the file */
    hsize_t     *count_out;          /* size of the hyperslab in memory */
    hsize_t     *offset_out;         /* hyperslab offset in memory */
    hsize_t     *dims_mem;              /* memory space dimensions */
    hsize_t     *dims_out;           /* dataset dimensions */
    int          i, j, k, rank;
    int          status_n;
    unsigned long length;
    unsigned long nbytes;

    void *value;

  /*
   *fprintf(stdout, "\n\n\nhit _NclHDF5get_native_dataset. file: %s, line: %d\n", __FILE__, __LINE__);
   *fprintf(stdout, "\tdataset_name: <%s>\n", dataset_name);
   */

    did = H5Dopen2(fid, dataset_name, H5P_DEFAULT);

    /*
     * Get datatype and dataspace handles and then query
     * dataset class, order, size, rank and dimensions.
     */
    datatype  = H5Dget_type(did);     /* datatype handle */
    t_class     = H5Tget_class(datatype);
  /*
   *if (t_class == H5T_INTEGER)
   *    fprintf(stdout, "\tData set has INTEGER type \n");
   */
    order     = H5Tget_order(datatype);
  /*
   *if (order == H5T_ORDER_LE)
   *    fprintf(stdout, "\tLittle endian order \n");
   */

    datasize  = H5Tget_size(datatype);

    dataspace = H5Dget_space(did);    /* dataspace handle */
    rank      = H5Sget_simple_extent_ndims(dataspace);

    count = calloc(rank, sizeof(hsize_t));
    offset = calloc(rank, sizeof(hsize_t));
    count_out = calloc(rank, sizeof(hsize_t));
    offset_out = calloc(rank, sizeof(hsize_t));
    dims_mem = calloc(rank, sizeof(hsize_t));
    dims_out = calloc(rank, sizeof(hsize_t));

    length = 1;

    status_n  = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);

    for(i = 0; i < rank; i++)
    {
        offset[i] = 0;
        offset_out[i] = 0;
        count[i] = dims_out[i];
        count_out[i] = dims_out[i];
        dims_mem[i] = dims_out[i];

        length *= dims_out[i];
    }

    /*
     * Define hyperslab in the dataset.
     */
    status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL,
                                 count, NULL);

    /*
     * Define the memory dataspace.
     */
    memspace = H5Screate_simple(rank,dims_mem,NULL);

    /*
     * Define memory hyperslab.
     */
    status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL,
                                 count_out, NULL);

    /*
     * Read data from hyperslab in the file into the hyperslab in
     * memory and display.
     */
    h5type = Ncl2HDF5type(type);

    if(h5type < 0)
    {
        fprintf(stdout, "\tDO NOT KNOW HOW TO read type: <%s>, file: %s, line: %d\n",
                         type, __FILE__, __LINE__);
    }
    else if(H5T_COMPOUND == h5type)
    {
        hid_t datatype_id = -1;
        hid_t component_datasize = 1;

        fprintf(stdout, "\tHANDLING type: <%s>, file: %s, line: %d\n",
                         type, __FILE__, __LINE__);

      /*
       *fprintf(stdout, "\tlength = %d\n", length);
       *fprintf(stdout, "\tdatatype = %d\n", datatype);
       *fprintf(stdout, "\tdatasize = %d\n", datasize);

       *fprintf(stdout, "\tdataset_name = %s\n", dataset_name);
       *fprintf(stdout, "\tdid = %d\n", did);
       *fprintf(stdout, "\tmemspace = %d\n", memspace);
       *fprintf(stdout, "\tdataspace = %d\n", dataspace);

       *fprintf(stdout, "\tcomponent: <%s>\n", component);
       *fprintf(stdout, "\tH5T_NATIVE_FLOAT: %d\n", H5T_NATIVE_FLOAT);
       */

        for(i = 0; i < compound->nom; i++)
        {
          /*
           *fprintf(stdout, "\tcompound->member[%d].name: <%s>\n",
           *                i, compound->member[i].name);
           *fprintf(stdout, "\tcompound->member[%d].type: <%s>\n",
           *                i, compound->member[i].type);
           *fprintf(stdout, "\tNcl2HDF5type(compound->member[%d].type): %d\n",
           *                i, Ncl2HDF5type(compound->member[i].type));
           *fprintf(stdout, "\tcompound->member[%d].offset: %d\n",
           *                i, compound->member[i].offset);
           */
            if(0 == strcmp(component, compound->member[i].name))
            {
                component_datasize = NclHDF5sizeof(compound->member[i].type);
                fprintf(stdout, "\tcomponent: <%s>\n", component);
                fprintf(stdout, "\tcompound->member[%d].type: <%s>\n",
                                i, compound->member[i].type);
                fprintf(stdout, "\tcomponent_datasize: %d\n", component_datasize);
              /*
               *fprintf(stdout, "\tH5T_NATIVE_FLOAT: %d\n", H5T_NATIVE_FLOAT);
               *fprintf(stdout, "\tNcl2HDF5type(compound->member[%d].type): %d\n",
               *                i, Ncl2HDF5type(compound->member[i].type));
               */
                datatype_id = H5Tcreate( H5T_COMPOUND, component_datasize);
                H5Tinsert(datatype_id, component, 0, 
                          Ncl2HDF5type(compound->member[i].type));
                break;
            }
        }
        nbytes = component_datasize * length;
        value = calloc(nbytes, sizeof(char));

        status = H5Dread(did, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, value);

      /*
       *{
       *float *fp = (float *) value;
       *for(i = 0; i < length; i += 201)
       *    fprintf(stdout, "%s[%d]: %f\n", component, i, fp[i]);
       *}
       */
    }
    else
    {
        nbytes = datasize * length;
        value = calloc(nbytes, sizeof(char));
        status = H5Dread(did, h5type, memspace, dataspace,
                         H5P_DEFAULT, value);
    }

    H5Tclose(datatype);
    H5Sclose(dataspace);
    H5Sclose(memspace);
    H5Dclose(did);

    if(count)
        free(count);
    if(offset)
        free(offset);
    if(count_out)
        free(count_out);
    if(offset_out)
        free(offset_out);
    if(dims_mem)
        free(dims_mem);
    if(dims_out)
        free(dims_out);

    return ((unsigned char *) value);
}


/*
 ***********************************************************************
 * Function:	_NclFree_HDF5external_link
 *
 * Purpose:	Free NclHDF5external_link
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclFree_HDF5external_link(NclHDF5external_link_t *NclHDF5external_link)
{
    NclHDF5external_link_t *curHDF5external_link;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclFree_HDF5external_link, in file: %s, at line: %d\n",
            __FILE__, __LINE__);
#endif

    while(NclHDF5external_link)
    {
        curHDF5external_link = NclHDF5external_link;
        NclHDF5external_link = curHDF5external_link->next;
        curHDF5external_link->next = NULL;

        free(curHDF5external_link);
    }
#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclFree_HDF5external_link, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclFree_HDF5attr_list
 *
 * Purpose:	Free NclHDF5attr_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclFree_HDF5attr_list(NclHDF5attr_list_t *NclHDF5attr_list)
{
    NclHDF5attr_list_t  *curHDF5attr_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclFree_HDF5attr_list, in file: %s, at line: %d\n",
            __FILE__, __LINE__);
#endif

    while(NclHDF5attr_list)
    {
        curHDF5attr_list = NclHDF5attr_list;
        NclHDF5attr_list = curHDF5attr_list->next;
        curHDF5attr_list->next = NULL;

        if(curHDF5attr_list->attr_node)
        {
            if(curHDF5attr_list->attr_node->value)
               free(curHDF5attr_list->attr_node->value);
            free(curHDF5attr_list->attr_node);
        }

        free(curHDF5attr_list);
    }
#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclFree_HDF5attr_list, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclFree_HDF5dataset_list
 *
 * Purpose:	Free NclHDF5dataset_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclFree_HDF5dataset_list(NclHDF5dataset_list_t *NclHDF5dataset_list)
{
    NclHDF5dataset_list_t  *curHDF5dataset_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclFree_HDF5dataset_list, in file: %s, at line: %d\n",
            __FILE__, __LINE__);
#endif

    while(NclHDF5dataset_list)
    {
        curHDF5dataset_list = NclHDF5dataset_list;
        NclHDF5dataset_list = curHDF5dataset_list->next;
        curHDF5dataset_list->next = NULL;

        if(curHDF5dataset_list->dataset_node)
        {
            if(curHDF5dataset_list->dataset_node->attr_list)
                _NclFree_HDF5attr_list(curHDF5dataset_list->dataset_node->attr_list);

            if(curHDF5dataset_list->dataset_node->compound)
            {
                if(curHDF5dataset_list->dataset_node->compound->nom)
                    free(curHDF5dataset_list->dataset_node->compound->member);
                free(curHDF5dataset_list->dataset_node->compound);
            }

            free(curHDF5dataset_list->dataset_node);
        }
        free(curHDF5dataset_list);
    }
#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclFree_HDF5dataset_list, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5free_group
 *
 * Purpose:	Free HDF5group
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclHDF5free_group(NclHDF5group_node_t *HDF5group)
{
#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5free_group, in file: %s, at line: %d\n",
            __FILE__, __LINE__);
#endif

    if(HDF5group)
    {
        NclHDF5group_list_t *curHDF5group_list;
       
        _NclFree_HDF5dataset_list(HDF5group->dataset_list);
        _NclFree_HDF5external_link(HDF5group->elink_list);
        _NclFree_HDF5attr_list(HDF5group->attr_list);

        while(HDF5group->group_list)
        {
            curHDF5group_list = HDF5group->group_list;
            HDF5group->group_list = curHDF5group_list->next;
            curHDF5group_list->next = NULL;

            _NclHDF5free_group(curHDF5group_list->group_node);
            free(curHDF5group_list);
        }

        free(HDF5group);
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5free_group, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclPrint_HDF5external_link
 *
 * Purpose:	Print NclHDF5external_link
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclPrint_HDF5external_link(NclHDF5external_link_t *NclHDF5external_link)
{
    NclHDF5external_link_t *curHDF5external_link;
    int nl = 0;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclPrint_HDF5external_link, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    curHDF5external_link = NclHDF5external_link;

    while(curHDF5external_link)
    {
        nl++;
        fprintf(stdout, "\tLink %d, path: <%s>\n", nl, curHDF5external_link->path);

        curHDF5external_link = curHDF5external_link->next;
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclPrint_HDF5external_link, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclPrint_HDF5attr_list
 *
 * Purpose:	Print NclHDF5attr_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclPrint_HDF5attr_list(NclHDF5attr_list_t *NclHDF5attr_list)
{
    NclHDF5attr_list_t  *curHDF5attr_list;
    NclHDF5attr_node_t  *attr_node;

    int na = 0;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclPrint_HDF5attr_list, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    curHDF5attr_list = NclHDF5attr_list;

    while(curHDF5attr_list)
    {
        na++;
        attr_node = curHDF5attr_list->attr_node;
        fprintf(stdout, "Attribute %d:\n", na);

#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tcounter: %d\n", attr_node->counter);
#endif
        fprintf(stdout, "\tid: %d\n", attr_node->id);
        fprintf(stdout, "\tname: <%s>\n", attr_node->name);
#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tdataspace: <%s>\n", attr_node->dataspace);
        fprintf(stdout, "\tspace: %d\n", attr_node->space);
        fprintf(stdout, "\ttype: %d\n", attr_node->type);
        fprintf(stdout, "\tspace_type: %d\n", attr_node->space_type);
#endif
        fprintf(stdout, "\ttype_name: <%s>\n", attr_node->type_name);
        fprintf(stdout, "\tndims: %d\n", attr_node->ndims);

        if(attr_node->ndims)
        {
            int i;
            for (i=0; i<attr_node->ndims; i++)
            {
                fprintf(stdout, "\tdims[%d] = %d\n", i, attr_node->dims[i]);
            }
        }

#if 0
        _NclHDF5Print_data_value(attr_node->value, attr_node->ndims,
                                 attr_node->dims, attr_node->type_name);
#endif

        curHDF5attr_list = curHDF5attr_list->next;
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclPrint_HDF5attr_list, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclPrint_HDF5dataset_list
 *
 * Purpose:	Print NclHDF5dataset_list
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclPrint_HDF5dataset_list(NclHDF5dataset_list_t *NclHDF5dataset_list)
{
    NclHDF5dataset_list_t  *curHDF5dataset_list;
    NclHDF5dataset_node_t  *dataset;
    int i = 0;
    int nd = 0;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclPrint_HDF5dataset_list, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    curHDF5dataset_list = NclHDF5dataset_list;

    while(curHDF5dataset_list)
    {
        dataset = curHDF5dataset_list->dataset_node;
        fprintf(stdout, "\n\n");
        fprintf(stdout, "Dataset %d:\n", nd);

        nd++;
        if(dataset->attr_list)
            _NclPrint_HDF5attr_list(dataset->attr_list);

        fprintf(stdout, "\tid=%d\n", dataset->id);
        fprintf(stdout, "\ttype_name=<%s>\n", dataset->type_name);
        fprintf(stdout, "\tname=<%s>\n", dataset->name);
        fprintf(stdout, "\tgroup_name=<%s>\n", dataset->group_name);
        fprintf(stdout, "\tshort_name=<%s>\n", dataset->short_name);
#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tspace=%d\n", dataset->space);
        fprintf(stdout, "\tspace_type=%d\n", dataset->space_type);
        fprintf(stdout, "\tspace_name=<%s>\n", dataset->space_name);
#endif
        fprintf(stdout, "\tndims=%d\n", dataset->ndims);
        for(i=0; i<dataset->ndims; i++)
        {
            fprintf(stdout, "\tdims[%d] = %d\n", i, dataset->dims[i]);
        }

        curHDF5dataset_list = curHDF5dataset_list->next;
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclPrint_HDF5dataset_list, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5print_group
 *
 * Purpose:	Print HDF5group
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */
void _NclHDF5print_group(NclHDF5group_node_t *HDF5group)
{
    NclHDF5group_list_t *curHDF5group_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5print_group, file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    if(HDF5group != NULL)
    {
        NclHDF5group_list_t    *HDF5group_list;
        int ng = 0;

        if(HDF5group->name)
        {
            fprintf(stdout, "Group name: <%s>\n", HDF5group->name);
        }

        fprintf(stdout, "\tGroup   id: %d\n", HDF5group->id);
        fprintf(stdout, "\ttype: %d\n", HDF5group->type);
        fprintf(stdout, "\ttype_name: <%s>\n", HDF5group->type_name);
       
        fprintf(stdout, "\tnum_attrs   : %d\n", HDF5group->num_attrs);
        if(HDF5group->attr_list)
            _NclPrint_HDF5attr_list(HDF5group->attr_list);

        fprintf(stdout, "\tnum_links   : %d\n", HDF5group->num_links);
        if(HDF5group->elink_list)
            _NclPrint_HDF5external_link(HDF5group->elink_list);

        fprintf(stdout, "\tnum_datasets: %d\n", HDF5group->num_datasets);
        if(HDF5group->dataset_list)
            _NclPrint_HDF5dataset_list(HDF5group->dataset_list);

        fprintf(stdout, "\tnum_groups  : %d\n", HDF5group->num_groups);

        curHDF5group_list = HDF5group->group_list;

        while(curHDF5group_list)
        {
            ng++;
            fprintf(stdout, "Group %d:\n", ng);

            if(curHDF5group_list->group_node)
            {
                _NclHDF5print_group(curHDF5group_list->group_node);
            }
            else
            {
                fprintf(stdout, "curHDF5group_list->group_node is NULL.\n");
            }

            curHDF5group_list = curHDF5group_list->next;
        }
    }
    else
    {
        fprintf(stdout, "NclHDF5file is NULL.\n");
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5print_group, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5Print_data_value
 *
 * Purpose:	Print data value
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 5, 2009
 *
 ***********************************************************************
 */

void _NclHDF5Print_data_value(void *value, int ndims, hsize_t *dims, char *type_name)
{
    int i, n, len;
    int width = 10;
    int m = ndims - 1;
    int step = 1;
    int iend = 1;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEnter _NclHDF5Print_data_value, for type_name: <%s>, at file: %s, line: %d\n",
            type_name, __FILE__, __LINE__);

    fprintf(stdout, "\tstrcmp(type_name, \"float\") = %d\n", strcmp(type_name, "float"));
    fprintf(stdout, "\tstrcmp(type_name, \"integer\") = %d\n", strcmp(type_name, "integer"));
    fprintf(stdout, "\tstrcmp(type_name, \"string\") = %d\n", strcmp(type_name, "string"));
#endif

    if(!value)
    {
        fprintf(stdout, "\tNo value to print.\n");
        return;
    }

    if(ndims)
    {
        if(dims[ndims-1] < width)
            width = dims[ndims-1];
    }

    if(ndims > 1)
    {
        len = dims[ndims-1];
        while(len > width)
        {
            step++;
            len = dims[ndims-1]/step;
        }
    }

    if(strcmp(type_name, "float") == 0)
    {
        float *fv = (float *) value;
        fprintf(stdout, "\tfloat value:");

        if(ndims < 1)
            fprintf(stdout, " %f\n", fv[0]);
        else if(ndims > 1)
        {
            m = 1;
            for(n = 0; n < ndims - 1; n++)
                m *= dims[n];

            len = 0;
            for(n = 0; n < m; n++)
            {
                for(i = 0; i < dims[ndims - 1]; i++)
                {
                    if(i%10 == 0)
                    {
                        iend = i+width-1;
                        if(iend >= (dims[ndims - 1] - 1))
                            iend = dims[ndims - 1] - 1;
                        fprintf(stdout, "\n\t[%3d, %3d -> %3d]", n, i, iend);
                    }
                    fprintf(stdout, " %f", fv[len + i]);
                }
                fprintf(stdout, "\n");
                len += dims[ndims - 1];
            }
            fprintf(stdout, "\n");
        }
        else
        {
            for(i = 0; i < dims[0]; i++)
            {
                if(i%10 == 0)
                {
                    iend = i+width-1;
                    if(iend >= (dims[0] - 1))
                        iend = dims[0] - 1;
                    fprintf(stdout, "\n\t[%3d -> %3d]", i, iend);
                }
                fprintf(stdout, " %f", fv[i]);
            }
            fprintf(stdout, "\n");
        }
    }
    else if(strcmp(type_name, "integer") == 0)
    {
        int *iv = (int *) value;

        fprintf(stdout, "\tinteger value: ndims=%d \n", ndims);

        if(ndims < 1)
            fprintf(stdout, "\tscalar, iv = %d\n", iv[0]);
        else if(ndims > 1)
        {
            m = 1;
            for(n = 0; n < ndims - 1; n++)
                m *= dims[n];

            len = 0;
            for(n = 0; n < m; n++)
            {
                for(i = 0; i < dims[ndims-1]; i++)
                {
                    if(i%10 == 0)
                    {
                        iend = i+width-1;
                        if(iend >= (dims[ndims - 1] - 1))
                            iend = dims[ndims - 1] - 1;
                        fprintf(stdout, "\n\t[%3d, %3d -> %3d]", n, i, iend);
                    }
                    fprintf(stdout, " %10d", iv[len + i]);
                }
                if(width == 10)
                    fprintf(stdout, "\n");
                len += dims[ndims-1];
            }
            fprintf(stdout, "\n");
        }
        else
        {
            for(i = 0; i < dims[0]; i++)
            {
                if(i%10 == 0)
                {
                    iend = i+width-1;
                    if(iend >= (dims[0] - 1))
                        iend = dims[0] - 1;
                    fprintf(stdout, "\n\t[%3d -> %3d]", i, iend);
                }
                fprintf(stdout, " %10d", iv[i]);
            }
            fprintf(stdout, "\n");
        }
    }
    else if(strcmp(type_name, "string") == 0)
    {
        char *cv = (char *) value;
        fprintf(stdout, "\tchar value: <%s>\n\n", cv);
    }
    else
    {
        fprintf(stdout, "\tUnable to print value for type_name: <%s>\n", type_name);
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leave _NclHDF5Print_data_value at file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
}


/*
 ***********************************************************************
 * Function:	_NclHDF5dataset_info
 *
 * Purpose:	find information about this dataset.
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */

herr_t _NclHDF5dataset_info(hid_t dset, char *name, NclHDF5dataset_node_t *dataset_node)
{
    hsize_t     cur_size[H5S_MAX_RANK];   /* current dataset dimensions */
    hsize_t     max_size[H5S_MAX_RANK];   /* maximum dataset dimensions */
    hid_t       space;          /* data space                 */
    int         ndims;          /* dimensionality             */
    H5S_class_t space_type;     /* type of dataspace          */
    int   i;

    NclHDF5datatype_t *NclHDF5datatype;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\n\n\nEnter _NclHDF5dataset_info at file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    space = H5Dget_space(dset);
    space_type = H5Sget_simple_extent_type(space);
    ndims = H5Sget_simple_extent_dims(space, cur_size, max_size);

    dataset_node->id = dset;
    strcpy(dataset_node->name, name);
    dataset_node->space = space;
    dataset_node->space_type = space_type;
    dataset_node->ndims = ndims;

    for (i=0; i<ndims; i++)
    {
        dataset_node->dims[i] = cur_size[i];
    }

    if (space_type==H5S_SCALAR)
        strcpy(dataset_node->space_name, "SCALAR");
    else if (space_type==H5S_NULL)
        strcpy(dataset_node->space_name, "NULL");
    else if (space_type==H5S_SIMPLE)
        strcpy(dataset_node->space_name, "SIMPLE");
    else
        strcpy(dataset_node->space_name, "DATA_ERROR");

    H5Sclose (space);

    dataset_node->type = H5Dget_type(dset);

    /* Data type */
    NclHDF5datatype = _NclHDF5get_typename(dataset_node->type, 15);

    strcpy(dataset_node->type_name, NclHDF5datatype->type_name);

    if(NclHDF5datatype->compound_nom)
    {
        dataset_node->compound = calloc(1, sizeof(NclHDF5compound_t));
        if(! dataset_node->compound)
        {
            fprintf(stdout, "UNABLE TO ALLOCATE MEMORY for dataset_node->compound, in file: %s, line: %d\n",
                    __FILE__, __LINE__);
            free(NclHDF5datatype);
            return FAILED;
        }
        dataset_node->compound->member = calloc(NclHDF5datatype->compound_nom,
                                                sizeof(NclHDF5compound_component_list_t));
        if(! dataset_node->compound->member)
        {
            fprintf(stdout, "UNABLE TO ALLOCATE MEMORY for dataset_node->compound->member, in file: %s, line: %d\n",
                    __FILE__, __LINE__);
            free(NclHDF5datatype);
            return FAILED;
        }

        dataset_node->compound->nom = NclHDF5datatype->compound_nom;
        dataset_node->compound->size = NclHDF5datatype->compound_size;

        for(i=0; i<NclHDF5datatype->compound_nom; i++)
        {
            strcpy(dataset_node->compound->member[i].name, NclHDF5datatype->compound_name[i]);
            strcpy(dataset_node->compound->member[i].type, NclHDF5datatype->compound_type[i]);
            dataset_node->compound->member[i].offset = NclHDF5datatype->compound_offset[i];
            dataset_node->compound->member[i].type_id = NclHDF5datatype->compound_type_id[i];
        }

#if DEBUG_NCL_HDF5
        fprintf(stdout, "\nTHIS IS COMPOUND data. file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tdataset_node->compound->nom = %d\n",
                dataset_node->compound->nom);
        fprintf(stdout, "\tdataset_node->compound->size= %d\n",
                dataset_node->compound->size);
        for(i=0; i<dataset_node->compound->nom; i++)
        {
            fprintf(stdout, "\tdataset_node->compound->member[%d].name: <%s>\n",
                    i, dataset_node->compound->member[i].name);
            fprintf(stdout, "\tdataset_node->compound->member[%d].type: <%s>\n",
                    i, dataset_node->compound->member[i].type);
            fprintf(stdout, "\tdataset_node->compound->member[%d].offset: %d\n",
                    i, dataset_node->compound->member[i].offset);
        }
#endif
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\n\tdataset_node->type_name=<%s>\n", dataset_node->type_name);
    fprintf(stdout, "\tdataset_node->id=%d\n", dataset_node->id);
    fprintf(stdout, "\tdataset_node->space=%d\n", dataset_node->space);
    fprintf(stdout, "\tdataset_node->space_type=%d\n", dataset_node->space_type);
    fprintf(stdout, "\tdataset_node->space_name=<%s>\n", dataset_node->space_name);
    fprintf(stdout, "\tdataset_node->ndims=%d\n", dataset_node->ndims);
    for(i=0; i<dataset_node->ndims; i++)
    {
        fprintf(stdout, "\tdataset_node->dims[%d] = %d\n", i, dataset_node->dims[i]);
    }
#endif

    free(NclHDF5datatype);

    _NclHDF5dataset_attr(dset, name, dataset_node);

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nLeave _NclHDF5dataset_info at file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif

    return SUCCEED;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5dataset_attr
 *
 * Purpose:	Check dataset attribute info.
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */

herr_t _NclHDF5dataset_attr(hid_t dset, char *name, NclHDF5dataset_node_t *dataset_node)
{
    hid_t       dcpl;           /* dataset creation property list */
    hid_t       type;           /* data type of dataset */
    hid_t       space;          /* data space of dataset */
    int         nf;             /* number of filters */
    unsigned    filt_flags;     /* filter flags */
    H5Z_filter_t filt_id;       /* filter identification number */
    unsigned    cd_values[20];  /* filter client data values */
    hsize_t     cd_nelmts;      /* filter client number of values */
    hsize_t     cd_num;         /* filter client data counter */
    char        f_name[256];    /* filter/file name */
    char        s[64];          /* temporary string buffer */
    off_t       f_offset;       /* offset in external file */
    hsize_t     f_size;         /* bytes used in external file */
    hsize_t     total, used;    /* total size or offset */
    int         ndims;          /* dimensionality */
    int         n, max_len;     /* max extern file name length */
    double      utilization;    /* percent utilization of storage */
    int   i;

    NclHDF5attr_node_t *attr_node;
    NclHDF5attr_list_t *curAttrList;
    NclHDF5datatype_t *NclHDF5datatype;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\n\nEnter _NclHDF5dataset_attr at file: %s, line: %d\n", __FILE__, __LINE__);
#endif

    curAttrList = calloc(1, sizeof(NclHDF5attr_list_t));
    if(!curAttrList)
    {
        fprintf(stdout, "Failed to allocated memory for curAttrList. in file: %s, line: %d\n",
                __FILE__, __LINE__);
        return FAILED;
    }

    curAttrList->next = dataset_node->attr_list;
    dataset_node->attr_list = curAttrList;

    curAttrList->attr_node = calloc(1, sizeof(NclHDF5attr_node_t));
    if(!curAttrList->attr_node)
    {
        fprintf(stdout, "Failed to allocated memory for curAttrList->attr_node. in file: %s, line: %d\n",
                __FILE__, __LINE__);
        return FAILED;
    }

    dcpl = H5Dget_create_plist(dset);
    space = H5Dget_space(dset);
    type = H5Dget_type(dset);

    attr_node = curAttrList->attr_node;

    attr_node->id = dcpl;
    attr_node->type = type;
    attr_node->space = space;
    strcpy(attr_node->name, name);

    /* Print information about chunked storage */
    if (H5D_CHUNKED==H5Pget_layout(dcpl))
    {
        hsize_t     chsize[64];     /* chunk size in elements */

        attr_node->ndims = H5Pget_chunk(dcpl, NELMTS(chsize), chsize/*out*/);

        total = H5Tget_size(type);
        for (i=0; i<attr_node->ndims; i++)
        {
            attr_node->dims[i] = chsize[i];
        }
    }

    /* Print total raw storage size */
    total = H5Sget_simple_extent_npoints(space) * H5Tget_size(type);
    used = H5Dget_storage_size(dset);

    if (used>0)
    {
        utilization = (total*100.0)/used;
    }

    /* Print information about external strorage */
    if((nf = H5Pget_external_count(dcpl)) > 0)
    {
        fprintf(stdout, "\n\n\nhit H5Pget_external_count. file: %s, line: %d\n", __FILE__, __LINE__);
#if 0
        for(i = 0, max_len = 0; i < nf; i++)
        {
            H5Pget_external(dcpl, (unsigned)i, sizeof(f_name), f_name, NULL, NULL);
            printf("%s", f_name);
        }

        printf("    %-10s %d external file%s\n",
                "Extern:", nf, 1==nf?"":"s");
        printf("        %4s %10s %10s %10s %s\n",
                "ID", "DSet-Addr", "File-Addr", "Bytes", "File");
        printf("        %4s %10s %10s %10s ",
                "----", "----------", "----------", "----------");
        for (i=0; i<max_len; i++) putchar('-');

        putchar('\n');
        for (i=0, total=0; i<nf; i++)
        {
            if (H5Pget_external(dcpl, (unsigned)i, sizeof(f_name), f_name, &f_offset, &f_size)<0)
            {
                fprintf(stdout,
                        "        #%03d %10Hu %10s %10s ***ERROR*** %s\n",
                        i, total, "", "",
                        i+1<nf?"Following addresses are incorrect":"");
            }
            else if (H5S_UNLIMITED==f_size)
            {
                fprintf(stdout, "        #%03d %10Hu %10Hu %10s ",
                        i, total, (hsize_t)f_offset, "INF");
                printf("%s", f_name);
            }
            else
            {
                fprintf(stdout, "        #%03d %10Hu %10Hu %10Hu ",
                        i, total, (hsize_t)f_offset, f_size);
                printf("%s", f_name);
            }
            putchar('\n');
            total += f_size;
        }

        printf("        %4s %10s %10s %10s ",
                "----", "----------", "----------", "----------");
        for (i=0; i<max_len; i++)
            putchar('-');
        putchar('\n');
#endif
    }

    /* Print information about raw data filters */
    if((nf = H5Pget_nfilters(dcpl)) > 0)
    {
        fprintf(stdout, "\n\n\nhit H5Pget_nfilters. file: %s, line: %d\n", __FILE__, __LINE__);
#if 1
        for(i = 0; i < nf; i++)
        {
            cd_nelmts = NELMTS(cd_values);
            filt_id = H5Pget_filter2(dcpl, (unsigned)i, &filt_flags, &cd_nelmts,
                    cd_values, sizeof(f_name), f_name, NULL);
            f_name[sizeof(f_name) - 1] = '\0';
            sprintf(s, "Filter-%d:", i);
            printf("    %-10s %s-%u %s {", s,
                    (f_name[0] ? f_name : "method"),
                    (unsigned)filt_id,
                    ((filt_flags & H5Z_FLAG_OPTIONAL) ? "OPT" : ""));
            for(cd_num = 0; cd_num < cd_nelmts; cd_num++)
                printf("%s%u", (cd_num ? ", " : ""), cd_values[cd_num]);
            printf("}\n");
        }
#endif
    }

    /* Data type */
    NclHDF5datatype = _NclHDF5get_typename(type, 15);

    strcpy(attr_node->type_name, NclHDF5datatype->type_name);

    /* Close stuff */
    H5Tclose(type);
    H5Sclose(space);
    H5Pclose(dcpl);

#if DEBUG_NCL_HDF5
#if 0
    _NclHDF5print_dataset(dset, NclHDF5datatype->type_name);
#endif
#endif

    free(NclHDF5datatype);

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nLeave _NclHDF5dataset_attr at file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif

    return SUCCEED;
}



/*
 ***********************************************************************
 * Function:	_NclHDF5search_by_name
 *
 * Purpose:	Iterator callback for traversing objects in file
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 2, 2009
 *
 ***********************************************************************
 */

herr_t _NclHDF5search_by_name(hid_t loc_id, char *path, H5L_info_t *linfo,
                              void *_udata)
{
    NclHDF5_ud_traverse_t *udata = (NclHDF5_ud_traverse_t *)_udata;     /* User data */
    char *new_name = NULL;
    char *full_name = NULL;
    char *already_visited = NULL; /* Whether the link/object was already visited */

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5search_by_name, path: <%s>, loc_id=%d, in file: %s, at line: %d\n",
            path, loc_id, __FILE__, __LINE__);
#endif

    /* Create the full path name for the link */
    if(udata->is_absolute)
    {
        hsize_t base_len = strlen(udata->base_grp_name);
        hsize_t add_slash = base_len ? ((udata->base_grp_name)[base_len-1] != '/') : 1;
            
        if(NULL == (new_name = malloc(base_len + add_slash + strlen(path) + 1)))
            return(H5_ITER_ERROR);
        strcpy(new_name, udata->base_grp_name);
        if (add_slash)
            new_name[base_len] = '/';
        strcpy(new_name + base_len + add_slash, path);
        full_name = strdup(new_name);
    }
    else
        full_name = strdup(path);

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\tpath: <%s>\n", path);
    fprintf(stdout, "\tfull_name: <%s>\n", full_name);
#endif

    /* Perform the correct action for different types of links */
    if(linfo->type == H5L_TYPE_HARD)
    {
        H5O_info_t oinfo;

        /* Get information about the object */
        if(H5Oget_info_by_name(loc_id, path, &oinfo, H5P_DEFAULT) < 0)
        {
            if(new_name)
                free(new_name);
                if(full_name)
                    free(full_name);
            return(H5_ITER_ERROR);
        }

        /* If the object has multiple links, add it to the list of addresses
         *  already visited, if it isn't there already
         */
        if(oinfo.rc > 1)
        {
            if(NULL == (already_visited = _NclHDF5_addr_visited(udata->seen, oinfo.addr)))
                _NclHDF5_addr_add(udata->seen, oinfo.addr, full_name);
        }

        /* Make 'visit object' callback */
        if(udata->searcher->_NclHDF5search_obj)
        {
            if((*udata->searcher->_NclHDF5search_obj)(full_name, &oinfo, udata->searcher->udata, already_visited) < 0)
            {
                if(new_name)
                    free(new_name);
                if(full_name)
                    free(full_name);
                return(H5_ITER_ERROR);
            }
        }
    }
    else
    {
        /* Make 'visit link' callback */
        if(udata->searcher->_NclHDF5search_link)
        {
            if((*udata->searcher->_NclHDF5search_link)(full_name, linfo, udata->searcher->udata) < 0)
            {
                if(new_name)
                    free(new_name);
                if(full_name)
                    free(full_name);
                return(H5_ITER_ERROR);
            }
        }
    }

    if(new_name)
        free(new_name);
    if(full_name)
        free(full_name);

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5search_by_name, in file: %s, at line: %d\n", __FILE__, __LINE__);
#endif

    return(H5_ITER_CONT);
}


/*
 ***********************************************************************
 * Function:	_NclHDF5_addr_add
 *
 * Purpose:	Recursively check a group and add a hardlink address to visited data structure
 *
 * Return:	void
 *
 * Programmer:	Wei Huang
 * Created:	July 1, 2009
 *
 ***********************************************************************
 */

void _NclHDF5_addr_add(NclHDF5_addr_t *visited, haddr_t addr, char *path)
{
    hsize_t idx;         /* Index of address to use */

    /* Allocate space if necessary */
    if(visited->nused == visited->nalloc)
    {
        if(visited->nalloc > 0)
            visited->nalloc *= 2;
        else
            visited->nalloc = 1;

        visited->objs = realloc(visited->objs, visited->nalloc * sizeof(visited->objs[0]));
    } /* end if */

    /* Append it */
    idx = visited->nused++;
    visited->objs[idx].addr = addr;
    strcpy(visited->objs[idx].path, path);
}


/*
 ***********************************************************************
 * Function:    _NclHDF5_addr_visited
 *
 * Purpose:     Recursively check a groupdd a hardlink address to visited data structure
 *
 * Return:      void
 *
 * Programmer:  Wei Huang
 * Created:     July 1, 2009
 *
 ***********************************************************************
 */

char *_NclHDF5_addr_visited(NclHDF5_addr_t *visited, haddr_t addr)
{
    hsize_t u;           /* Local index variable */

    /* Look for address */
    for(u = 0; u < visited->nused; u++)
    {
        /* Check for address already in array */
        if(visited->objs[u].addr == addr)
            return(visited->objs[u].path);
    }

    /* Didn't find address */
    return(NULL);
}


/*
 ***********************************************************************
 * Function:	_NclHDF5set_endian
 *
 * Purpose:	Set HDF5 endian, based on type.
 *
 * Programmer:	Wei Huang
 * Created:	July 1, 2009
 *
 ***********************************************************************
 */

void setHDF5endian(hid_t type, hsize_t size, NclHDF5datatype_t *NclHDF5datatype)
{
    H5T_order_t order;          /* byte order value */

   /* Byte order */
    if(size > 1)
    {
        order = H5Tget_order(type);
        if (H5T_ORDER_LE==order)
        {
            strcpy(NclHDF5datatype->endian, "little-endian");
        }
        else if (H5T_ORDER_BE==order)
        {
            strcpy(NclHDF5datatype->endian, "big-endian");
        }
        else if (H5T_ORDER_VAX==order)
        {
            strcpy(NclHDF5datatype->endian, "mixed-endian");
        }
        else
        {
            strcpy(NclHDF5datatype->endian, "unknown-byte-order");
        }
    }
}


/*
 ***********************************************************************
 * Function:	_NclHDF5get_typename
 *
 * Purpose:	Get typename based on type
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 1, 2009
 *
 ***********************************************************************
 */

NclHDF5datatype_t *_NclHDF5get_typename(hid_t type, int ind)
{
    H5T_class_t type_class;
    hsize_t     size;

    NclHDF5datatype_t *NclHDF5datatype;

    NclHDF5datatype = calloc(1, sizeof(NclHDF5datatype_t));
    if (! NclHDF5datatype)
    {
        fprintf(stdout, "Error to allocate memory for NclHDF5datatype, in file: %s, line: %d\n",
                __FILE__, __LINE__);
        return NULL;
    }

    NclHDF5datatype->type_name[0] = '\0';
    NclHDF5datatype->endian[0] = '\0';
    NclHDF5datatype->format[0] = '\0';
    NclHDF5datatype->bit = 0;
    NclHDF5datatype->usign = 0;
    NclHDF5datatype->ndims = 0;

    /* Bad data type */
    if(type < 0)
    {
        fprintf(stdout, "\n\t********************************************\n");
        fprintf(stdout, "\tBad data type: %d\n", type);
        fprintf(stdout, "\t<ERROR ERROR ERROR ERROR ERROR ERROR ERROR>\n");
        fprintf(stdout, "\tBad data type: %d\n", type);
        fprintf(stdout, "\t********************************************\n\n");
        return NclHDF5datatype;
    }

    /* Shared? If so then print the type's OID */
    if(H5Tcommitted(type))
    {
        H5O_info_t  oi;

        if(H5Oget_info(type, &oi) >= 0)
        {
            fprintf(stdout, "shared-%lu:"H5_PRINTF_HADDR_FMT" ",
                    oi.fileno, oi.addr);
            strcpy(NclHDF5datatype->type_name, "shared");
        }
        else
        {
            strcpy(NclHDF5datatype->type_name, "shared");
        }

        return NclHDF5datatype;
    }

    if (H5Tequal(type, H5T_IEEE_F32BE)==TRUE)
    {
        NclHDF5datatype->bit = 32;
        strcpy(NclHDF5datatype->type_name, "float");
        strcpy(NclHDF5datatype->endian, "big-endian");
        strcpy(NclHDF5datatype->format, "IEEE");
        return NclHDF5datatype;
    }
    else if (H5Tequal(type, H5T_IEEE_F32LE)==TRUE)
    {
        NclHDF5datatype->bit = 32;
        strcpy(NclHDF5datatype->type_name, "float");
        strcpy(NclHDF5datatype->endian, "little-endian");
        strcpy(NclHDF5datatype->format, "IEEE");
        return NclHDF5datatype;
    }
    else if (H5Tequal(type, H5T_IEEE_F64BE)==TRUE)
    {
        NclHDF5datatype->bit = 64;
        strcpy(NclHDF5datatype->type_name, "double");
        strcpy(NclHDF5datatype->endian, "big-endian");
        strcpy(NclHDF5datatype->format, "IEEE");
        return NclHDF5datatype;
    }
    else if (H5Tequal(type, H5T_IEEE_F64LE)==TRUE)
    {
        NclHDF5datatype->bit = 64;
        strcpy(NclHDF5datatype->type_name, "double");
        strcpy(NclHDF5datatype->endian, "little-endian");
        strcpy(NclHDF5datatype->format, "IEEE");
        return NclHDF5datatype;
    }
#if 0
    else
    {
        fprintf(stdout, "\n\tfile: %s, line: %d\n\n", __FILE__, __LINE__);
        fprintf(stdout, "\tNOT a Simple Type.\n");
    }
#endif

    size = H5Tget_size(type);
    NclHDF5datatype->bit = (unsigned) (8*size);

    type_class = H5Tget_class(type);

    switch (type_class)
    {
        case H5T_COMPOUND:
            {
                char        *name=NULL;     /* member name */
                hid_t       subtype;        /* member data type */
                unsigned    nmembs;         /* number of members */
                unsigned    i;              /* miscellaneous counters */

                NclHDF5datatype_t *COMPOUNDdatatype;
                COMPOUNDdatatype = calloc(1, sizeof(NclHDF5datatype_t));
                if (! COMPOUNDdatatype)
                {
                    fprintf(stdout, "Error to allocate memory for COMPOUNDdatatype, in file: %s, line: %d\n",
                            __FILE__, __LINE__);
                    return NULL;
                }

              /*
               *fprintf(stdout, "\n\tFound compounddata, file: %s, line: %d\n\n", __FILE__, __LINE__);
               */

                strcpy(NclHDF5datatype->type_name, "compound");

                nmembs=H5Tget_nmembers(type);

                NclHDF5datatype->compound_nom = nmembs;

                for (i=0; i<nmembs; i++)
                {
                    /* Name and offset */
                    name = H5Tget_member_name(type, i);
                    strcpy(NclHDF5datatype->compound_name[i], name);
                    NclHDF5datatype->compound_offset[i] =
                           (unsigned long) H5Tget_member_offset(type, i);
                    free(name);
            
                    /* Member's type */
                    subtype = H5Tget_member_type(type, i);
                    NclHDF5datatype->compound_type_id[i] = subtype;
                    COMPOUNDdatatype = _NclHDF5get_typename(subtype, ind+4);
                    strcpy(NclHDF5datatype->compound_type[i], COMPOUNDdatatype->type_name);
                    H5Tclose(subtype);

                  /*
                   *fprintf(stdout, "\tNclHDF5datatype->compound_name[%d]: <%s>\n", 
                   *        i, NclHDF5datatype->compound_name[i]);
                   *fprintf(stdout, "\tNclHDF5datatype->compound_type[%d]: <%s>\n", 
                   *        i, NclHDF5datatype->compound_type[i]);
                   *fprintf(stdout, "\tNclHDF5datatype->compound_.offset[%d]: <%d>\n", 
                   *        i, NclHDF5datatype->compound_offset[i]);
                   *fprintf(stdout, "\tNclHDF5datatype->compound_.type_id[%d]: <%d>\n", 
                   *        i, NclHDF5datatype->compound_type_id[i]);
                   */
                }
                size = H5Tget_size(type);

                NclHDF5datatype->bit = size;
                NclHDF5datatype->compound_size = size;

              /*
               *fprintf(stdout, "\tNclHDF5datatype->compound_nom = %d\n",
               *        NclHDF5datatype->compound_nom);
               *fprintf(stdout, "\tNclHDF5datatype->compound_size= %d\n",
               *        NclHDF5datatype->compound_size);
               */

                free(COMPOUNDdatatype);
            }
            return NclHDF5datatype;
            break;
        case H5T_INTEGER:
            {
                H5T_sign_t  sign;           /* sign scheme value */
                const char  *sign_s=NULL;   /* sign scheme string */
                char  var_name[32];

                strcpy(NclHDF5datatype->type_name, "integer");
                setHDF5endian(type, size, NclHDF5datatype);

                /* Sign */
                if ((sign=H5Tget_sign(type))>=0)
                {
                    if (H5T_SGN_NONE==sign)
                    {
                        sign_s = "unsigned";
                        NclHDF5datatype->usign = 1;
                        strcpy(NclHDF5datatype->type_name, "uint");
                    }
                    else if (H5T_SGN_2==sign)
                    {
                        sign_s = "";
                    }
                    else
                    {
                        sign_s = "unknown-sign";
                    }
                }
                else
                {
                    sign_s = "unknown-sign";
                }

                if(size == (hsize_t) sizeof(char))
                {
                    if(1 == NclHDF5datatype->usign)
                        strcpy(var_name, "byte");
                    else
                        strcpy(var_name, "char");
                }
                else if(size == (hsize_t) sizeof(short))
                {
                    if(1 == NclHDF5datatype->usign)
                        strcpy(var_name, "ushort");
                    else
                        strcpy(var_name, "short");
                }
                else if(size == (hsize_t) sizeof(int))
                {
                    if(1 == NclHDF5datatype->usign)
                        strcpy(var_name, "uint");
                    else
                        strcpy(var_name, "integer");
                }
                else if(size == (hsize_t) sizeof(long))
                {
                    if(1 == NclHDF5datatype->usign)
                        strcpy(var_name, "ulong");
                    else
                        strcpy(var_name, "long");
                }
                else if(size == (hsize_t) sizeof(long long))
                {
                    if(1 == NclHDF5datatype->usign)
                        strcpy(var_name, "uint64");
                    else
                        strcpy(var_name, "int64");
                }
                else
                {
                    fprintf(stdout, "file: %s, line: %d\n", __FILE__, __LINE__);
                    fprintf(stdout, "size: <%d>\n", size);
                    strcpy(var_name, "unknown integer");
                }
                strcpy(NclHDF5datatype->type_name, var_name);
            }
            return NclHDF5datatype;
            break;
        case H5T_FLOAT:
            {
                size_t      spos;           /* sign bit position */
                size_t      esize, epos;    /* exponent size and position */
                size_t      msize, mpos;    /* significand size and position */
                size_t      ebias;          /* exponent bias */
                H5T_norm_t  norm;           /* significand normalization */
                const char  *norm_s=NULL;   /* normalization string */
                H5T_pad_t   pad;            /* internal padding value */
                const char  *pad_s=NULL;    /* internal padding string */

                strcpy(NclHDF5datatype->type_name, "float");
                setHDF5endian(type, size, NclHDF5datatype);

                /* Print sizes, locations, and other information about each field */
                H5Tget_fields (type, &spos, &epos, &esize, &mpos, &msize);
                ebias = H5Tget_ebias(type);
                norm = H5Tget_norm(type);
                switch (norm)
                {
                    case H5T_NORM_IMPLIED:
                        norm_s = ", msb implied";
                        break;
                    case H5T_NORM_MSBSET:
                        norm_s = ", msb always set";
                        break;
                    case H5T_NORM_NONE:
                        norm_s = ", no normalization";
                        break;
                    case H5T_NORM_ERROR:
                        norm_s = ", unknown normalization";
                        break;
                }
                printf("\n%*s(significant for %lu bit%s at bit %lu%s)", ind, "",
                        (unsigned long)msize, 1==msize?"":"s", (unsigned long)mpos,
                        norm_s);
                printf("\n%*s(exponent for %lu bit%s at bit %lu, bias is 0x%lx)",
                        ind, "", (unsigned long)esize, 1==esize?"":"s",
                        (unsigned long)epos, (unsigned long)ebias);
                printf("\n%*s(sign bit at %lu)", ind, "", (unsigned long)spos);
    
                /* Display internal padding */
                if (1+esize+msize<H5Tget_precision(type))
                {
                    pad = H5Tget_inpad(type);
                    switch (pad)
                    {
                        case H5T_PAD_ZERO:
                            pad_s = "zero";
                            break;
                        case H5T_PAD_ONE:
                            pad_s = "one";
                            break;
                        case H5T_PAD_BACKGROUND:
                            pad_s = "bkg";
                            break;
                        case H5T_PAD_ERROR:
                        case H5T_NPAD:
                            pad_s = "unknown";
                            break;
                    }
                }
                printf("\n%*s(internal padding bits are %s)", ind, "", pad_s);
            }
            return NclHDF5datatype;
            break;
        case H5T_ENUM:
            {
                char        **name=NULL;    /* member names */
                unsigned char *value=NULL;  /* value array */
                unsigned char *copy = NULL; /* a pointer to value array */
                unsigned    nmembs;         /* number of members */
                int         nchars;         /* number of output characters */
                hid_t       super;          /* enum base integer type */
                hid_t       native=-1;      /* native integer data type */
                hsize_t     dst_size;       /* destination value type size */
                unsigned    i;              /* miscellaneous counters */
                hsize_t     j;

                strcpy(NclHDF5datatype->type_name, "enum");

                nmembs = H5Tget_nmembers(type);
                assert(nmembs>0);
                super = H5Tget_super(type);
                printf("enum ");
                _NclHDF5get_typename(super, ind+4);
                printf(" {");

                /* Determine what data type to use for the native values.  To simplify
                 * things we entertain three possibilities:
                 *  1. long_long -- the largest native signed integer
                 * 2. unsigned long_long -- the largest native unsigned integer
                 *     3. raw format */
                if (H5Tget_size(type)<=sizeof(long long))
                {
                    dst_size = sizeof(long long);
                    if (H5T_SGN_NONE==H5Tget_sign(type))
                    {
                        native = H5T_NATIVE_ULLONG;
                    }
                    else
                    {
                        native = H5T_NATIVE_LLONG;
                    }
                }
                else
                {
                    dst_size = size;
                }

                NclHDF5datatype->bit = (unsigned) (8*dst_size);

                /* Get the names and raw values of all members */
                name = calloc(nmembs, sizeof(char*));
                if(dst_size > size)
                    value = calloc(nmembs, dst_size);
                else
                    value = calloc(nmembs, size);

                for (i=0; i<nmembs; i++)
                {
                    name[i] = H5Tget_member_name(type, i);
                    H5Tget_member_value(type, i, value+i*H5Tget_size(type));
                }

                /* Convert values to native data type */
                if (native>0) H5Tconvert(super, native, nmembs, value, NULL, H5P_DEFAULT);

                /* Sort members by increasing value */
                /*not implemented yet*/

                /* Print members */
                for (i=0; i<nmembs; i++)
                {
                    printf("\n%*s", ind+4, "");
                    printf("%s = ", name[i]);

                    if (native<0)
                    {
                        printf("0x");
                        for (j=0; j<dst_size; j++)
                            printf("%02x", value[i*dst_size+j]);
                    }
                    else if (H5T_SGN_NONE==H5Tget_sign(native))
                    {
 	                /*On SGI Altix(cobalt), wrong values were printed out with "value+i*dst_size"
 	                 *strangely, unless use another pointer "copy".*/
 	                copy = value+i*dst_size;
                        fprintf(stdout,"%"H5_PRINTF_LL_WIDTH"u",
                        *((unsigned long long*)((void*)copy)));
                    }
                    else
                    {
 	                /*On SGI Altix(cobalt), wrong values were printed out with "value+i*dst_size"
 	                 *strangely, unless use another pointer "copy".*/
 	                copy = value+i*dst_size;
                        fprintf(stdout,"%"H5_PRINTF_LL_WIDTH"d",
                        *((long long*)((void*)copy)));
                    }
                }

                /* Release resources */
                for (i=0; i<nmembs; i++) free(name[i]);
                free(name);
                free(value);
                H5Tclose(super);

                if (0==nmembs) printf("\n%*s <empty>", ind+4, "");
                printf("\n%*s}", ind, "");
            }
            return NclHDF5datatype;
            break;
        case H5T_STRING:
            {
                H5T_str_t  pad;
                const char  *pad_s=NULL;
                H5T_cset_t  cset;
                const char  *cset_s=NULL;

                strcpy(NclHDF5datatype->type_name, "string");

                /* Padding */
                pad = H5Tget_strpad(type);
                switch (pad)
                {
                    case H5T_STR_NULLTERM:
                        pad_s = "null-terminated";
                        break;
                    case H5T_STR_NULLPAD:
                        pad_s = "null-padded";
                        break;
                    case H5T_STR_SPACEPAD:
                        pad_s = "space-padded";
                        break;
                    case H5T_STR_RESERVED_3:
                    case H5T_STR_RESERVED_4:
                    case H5T_STR_RESERVED_5:
                    case H5T_STR_RESERVED_6:
                    case H5T_STR_RESERVED_7:
                    case H5T_STR_RESERVED_8:
                    case H5T_STR_RESERVED_9:
                    case H5T_STR_RESERVED_10:
                    case H5T_STR_RESERVED_11:
                    case H5T_STR_RESERVED_12:
                    case H5T_STR_RESERVED_13:
                    case H5T_STR_RESERVED_14:
                    case H5T_STR_RESERVED_15:
                    case H5T_STR_ERROR:
                        pad_s = "unknown-format";
                        break;
                }
            
                /* Character set */
                cset = H5Tget_cset(type);
                switch (cset)
                {
                    case H5T_CSET_ASCII:
                        cset_s = "ASCII";
                        break;
                    case H5T_CSET_UTF8:
                        cset_s = "UTF-8";
                        break;
                    case H5T_CSET_RESERVED_2:
                    case H5T_CSET_RESERVED_3:
                    case H5T_CSET_RESERVED_4:
                    case H5T_CSET_RESERVED_5:
                    case H5T_CSET_RESERVED_6:
                    case H5T_CSET_RESERVED_7:
                    case H5T_CSET_RESERVED_8:
                    case H5T_CSET_RESERVED_9:
                    case H5T_CSET_RESERVED_10:
                    case H5T_CSET_RESERVED_11:
                    case H5T_CSET_RESERVED_12:
                    case H5T_CSET_RESERVED_13:
                    case H5T_CSET_RESERVED_14:
                    case H5T_CSET_RESERVED_15:
                    case H5T_CSET_ERROR:
                        cset_s = "unknown-character-set";
                        break;
                }

                if (H5Tis_variable_str(type))
                {
                    printf("variable-length");
                    NclHDF5datatype->bit = 0;
                }
                else
                {
#if 0
                    printf("%lu-byte", (unsigned long)size);
#endif
                    NclHDF5datatype->bit = (unsigned) (8*size);
                }
#if 0
                printf(" %s %s string", pad_s, cset_s);
#endif
            }
            return NclHDF5datatype;
            break;
        case H5T_REFERENCE:
            if (H5Tequal(type, H5T_STD_REF_OBJ))
            {
#if 0
                fprintf(stdout, "object reference, file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
                strcpy(NclHDF5datatype->type_name, "object reference");
            }
            else if (H5Tequal(type, H5T_STD_REF_DSETREG))
            {
                fprintf(stdout, "file: %s, line: %d\n\n", __FILE__, __LINE__);
                fprintf(stdout, "dataset region reference\n");
                strcpy(NclHDF5datatype->type_name, "dataset region reference");
            }
            else
            {
                fprintf(stdout, "file: %s, line: %d\n\n", __FILE__, __LINE__);
                strcpy(NclHDF5datatype->type_name, "unknown reference");
                fprintf(stdout, "%lu-byte unknown reference\n", (unsigned long)size);
                NclHDF5datatype->bit = (unsigned) (8*size);
            }

            return NclHDF5datatype;
            break;
        case H5T_OPAQUE:
            {
                char *tag;

                fprintf(stdout, "file: %s, line: %d\n\n", __FILE__, __LINE__);
                strcpy(NclHDF5datatype->type_name, "opaque");
                NclHDF5datatype->bit = (unsigned) (8*size);
                fprintf(stdout, "%lu-byte opaque type\n", (unsigned long)size);
                if ((tag=H5Tget_tag(type)))
                {
                    printf("\n%*s(tag = \"", ind, "");
                    printf("%s\")", tag);
                    strcpy(NclHDF5datatype->format, tag);
                    free(tag);
                }
            }
            return NclHDF5datatype;
            break;
        case H5T_VLEN:
            {
                hid_t       super;

                fprintf(stdout, "file: %s, line: %d\n\n", __FILE__, __LINE__);
                strcpy(NclHDF5datatype->type_name, "vlen");

                fprintf(stdout, "variable length of\n%*s\n", ind+4, "");
                super = H5Tget_super(type);
                _NclHDF5get_typename(super, ind+4);
                H5Tclose(super);
            }
            return NclHDF5datatype;
            break;
        case H5T_ARRAY:
            {
                hid_t       super;
                int         ndims, i;
                hsize_t     *dims=NULL;

                strcpy(NclHDF5datatype->type_name, "array");

                ndims = H5Tget_array_ndims(type);
                NclHDF5datatype->ndims = ndims;
                if (ndims)
                {
                    dims = malloc(ndims*sizeof(dims[0]));
                    H5Tget_array_dims2(type, dims);

                    /* Print dimensions */
                    for (i=0; i<ndims; i++)
                    {
                        fprintf(stdout, "%s%Hu" , i?",":"[", dims[i]);
                        NclHDF5datatype->dims[i] = dims[i];
                    }
                    putchar(']');

                    free(dims);
                }
                else
                {
                    strcpy(NclHDF5datatype->type_name, "scalar");
                    fputs(" [SCALAR]", stderr);
                }

                /* Print parent type */
                putchar(' ');
                super = H5Tget_super(type);
                _NclHDF5get_typename(super, ind+4);
                H5Tclose(super);
            }
            return NclHDF5datatype;
            break;
        case H5T_BITFIELD:
            strcpy(NclHDF5datatype->type_name, "bitfield");
            setHDF5endian(type, size, NclHDF5datatype);

            strcpy(NclHDF5datatype->endian, NclHDF5datatype->endian);
            NclHDF5datatype->bit = (unsigned) (8*size);
        
            printf("%lu-bit %s bitfield",
                   (unsigned long)(8*H5Tget_size(type)), NclHDF5datatype->endian);
#if 0
            display_precision(type, ind);
#endif

            return NclHDF5datatype;
            break;
        default:
            printf("Unknon type_class in file: %s, line: %d\n", __FILE__, __LINE__);
            /* Unknown type */
            printf("%lu-byte class-%u unknown",
                    (unsigned long)H5Tget_size(type), (unsigned)type_class);
            return NclHDF5datatype;
            break;
    }
}


/*
 *************************************************************************
 * Function:	_NclHDF5check_attr
 *
 * Purpose:	Check information about attributes.
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 *              July 1, 2009
 *
 *************************************************************************
 */

herr_t _NclHDF5check_attr(hid_t obj_id, char *attr_name, const H5A_info_t *ainfo,
                          void *attr_data)
{
    hid_t       attr_id, space, type, p_type;
    hsize_t     size[H5S_MAX_RANK], nelmts = 1;
    hsize_t     temp_need;
    H5S_class_t space_type;
    H5T_class_t type_class;
    int  ndims, i, n;
    hsize_t need;
  
    NclHDF5attr_list_t **attr_list = (NclHDF5attr_list_t **) attr_data;

    NclHDF5attr_list_t *cur_attr_list;
    NclHDF5attr_node_t *attr_node;

    NclHDF5datatype_t *NclHDF5datatype;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5check_attr, attr_name = <%s>, at file: %s, line: %d\n", attr_name, __FILE__, __LINE__);
#endif

    attr_id = H5Aopen(obj_id, attr_name, H5P_DEFAULT);

    if(attr_id < 0)
    {
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "PROBLEM TO FIND ATTRIBUTE in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tobj_id: %d\n", obj_id);
        fprintf(stdout, "\tattr_name: <%s>\n", attr_name);
        fprintf(stdout, "\tattr_id: %d\n", attr_id);
        fprintf(stdout, "PROBLEM TO FIND ATTRIBUTE in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "Leaving _NclHDF5check_attr, at file: %s, line: %d\n\n", __FILE__, __LINE__);
        H5Aclose(attr_id);
        return FAILED;
    }

    type = H5Aget_type(attr_id);

    if(type < 0)
    {
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "PROBLEM TO FIND ATTRIBUTE in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tobj_id: %d\n", obj_id);
        fprintf(stdout, "\tattr_name: <%s>\n", attr_name);
        fprintf(stdout, "\tattr_id: %d\n", attr_id);
        fprintf(stdout, "\ttype: %d\n", type);
        fprintf(stdout, "PROBLEM TO FIND ATTRIBUTE in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "\n\n\n");
        H5Tclose(type);
        H5Aclose(attr_id);

        fprintf(stdout, "Leaving _NclHDF5check_attr, at file: %s, line: %d\n\n", __FILE__, __LINE__);
        return FAILED;
    }

    space = H5Aget_space(attr_id);

    cur_attr_list = (NclHDF5attr_list_t *)malloc(sizeof(NclHDF5attr_list_t));
    cur_attr_list->next = NULL;

    cur_attr_list->attr_node = calloc(1, sizeof(NclHDF5attr_node_t));
    if(!cur_attr_list->attr_node)
    {
        fprintf(stdout, "Failed to allocated memory for cur_attr_list->attr_node. in file: %s, line: %d\n",
                __FILE__, __LINE__);
        return FAILED;
    }
    
    attr_node = cur_attr_list->attr_node;

    attr_node->id = obj_id;
    strcpy(attr_node->name, attr_name);

    attr_node->type = type;
    attr_node->space = space;

    attr_node->counter = 1;

    if(*attr_list)
    {
        attr_node->counter += (*attr_list)->attr_node->counter;
    }

    /* Data space */
    ndims = H5Sget_simple_extent_dims(space, size, NULL);

    if(ndims > 10)
    {
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "PROBLEM WITH NDIMS in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tobj_id: %d\n", obj_id);
        fprintf(stdout, "\tattr_name: <%s>\n", attr_name);
        fprintf(stdout, "\tattr_id: %d\n", attr_id);
        fprintf(stdout, "\tndims: %d\n", ndims);
        fprintf(stdout, "PROBLEM WITH NDIMS in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "\n\n\n");

        H5Tclose(type);
        H5Aclose(attr_id);

        attr_node->ndims = 0;

        _NclFree_HDF5attr_list(cur_attr_list);

        fprintf(stdout, "Leaving _NclHDF5check_attr, at file: %s, line: %d\n\n", __FILE__, __LINE__);

        return FAILED;
    }

    space_type = H5Sget_simple_extent_type(space);

    attr_node->ndims = ndims;
    attr_node->space_type = space_type;

    switch(space_type)
    {
        case H5S_SCALAR:
            /* scalar dataspace */
            strcpy(attr_node->dataspace, "scalar");
            break;
        case H5S_SIMPLE:
            /* simple dataspace */
            strcpy(attr_node->dataspace, "simple");
            for (i=0; i<ndims; i++)
            {
                attr_node->dims[i] = size[i];
                nelmts *= size[i];
            }
            break;
        case H5S_NULL:
            /* null dataspace */
            strcpy(attr_node->dataspace, "null");
            break;

        default:
            /* Unknown dataspace type */
            strcpy(attr_node->dataspace, "unknown");
            break;
    }

    /* Data type */
    NclHDF5datatype = _NclHDF5get_typename(type, 15);

    strcpy(attr_node->type_name, NclHDF5datatype->type_name);

    /* values of type reference */
    type_class = H5Tget_class(type);
    if(type_class==H5T_BITFIELD)
        p_type=H5Tcopy(type);
    else
        p_type = H5Tget_native_type(type,H5T_DIR_DEFAULT);

    attr_node->p_type = p_type;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\tfile: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stdout, "\tattr_node->counter: %d\n", attr_node->counter);
    fprintf(stdout, "\tattr_node->id: %d\n", attr_node->id);
    fprintf(stdout, "\tattr_node->name: <%s>\n", attr_node->name);
    fprintf(stdout, "\tattr_node->dataspace: <%s>\n", attr_node->dataspace);
    fprintf(stdout, "\tattr_node->space: %d\n", attr_node->space);
    fprintf(stdout, "\tattr_node->type: %d\n", attr_node->type);
    fprintf(stdout, "\tattr_node->p_type: %d\n", attr_node->p_type);
    fprintf(stdout, "\tattr_node->space_type: %d\n", attr_node->space_type);
    fprintf(stdout, "\tattr_node->type_name: <%s>\n", attr_node->type_name);
    fprintf(stdout, "\tattr_node->ndims: %d\n", attr_node->ndims);

    if(attr_node->ndims)
    {
        for (i=0; i<ndims; i++)
        {
            fprintf(stdout, "\tattr_node->dims[%d] = %d\n", i, attr_node->dims[i]);
        }
    }
#endif

    if(p_type >= 0)
    {
        hsize_t size = H5Tget_size(type);
        hsize_t p_size = H5Tget_size(p_type);
        if(p_size > size)
            temp_need = nelmts * p_size;
        else
            temp_need = nelmts * size;

        assert(temp_need == (hsize_t)((size_t)temp_need));
        need = (size_t)temp_need;
        if(0 == strcmp(attr_node->type_name, "string"))
            attr_node->nbytes = need + 1;
        else
            attr_node->nbytes = need;
        attr_node->value = malloc(need);
        assert(attr_node->value);

        n = H5Aread(attr_id, p_type, attr_node->value);
#if DEBUG_NCL_HDF5
        fprintf(stdout, "\tfile: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tn=%d\n", n);
        fprintf(stdout, "\tneed=%d\n", need);
        fprintf(stdout, "\tsize=%d\n", size);
        fprintf(stdout, "\tp_size=%d\n", p_size);
        fprintf(stdout, "\tnelmts=%d\n", nelmts);
        fprintf(stdout, "\tattr_node->type_name=<%s>\n", attr_node->type_name);

#if 0
        if(n == 0)
        {
            _NclHDF5Print_data_value(attr_node->value, attr_node->ndims,
                                     attr_node->dims, attr_node->type_name);
        }
#endif
#endif

        if((0 == strcmp(attr_node->type_name, "string")) && (attr_node->ndims))
        {
            int m, j;
            char str[HDF5_BUF_SIZE];
            char *osp;
            char *csp;
            char *new_str;

            osp = (int *)attr_node->value;

            new_str = malloc(attr_node->nbytes);
            n = 0;
            j = 0;
            for(m = 0; m < attr_node->ndims; m++)
            {
                for(i=0; i<attr_node->dims[m]; i++)
                {
                    csp = osp + j*size;
                    n += 1 + strlen(csp);

                    if(j)
                        strcat(new_str, csp);
                    else
                        strcpy(new_str, csp);
                    strcat(new_str, ",");
                    j++;
                }
            }
            new_str[n-1] = '\0';
            free(attr_node->value);
            attr_node->value = new_str;
            attr_node->nbytes = n;

            attr_node->nbytes = n;
        }

        if(0 == strcmp(attr_node->type_name, "object reference"))
        {
            int m, j;
            char str[HDF5_BUF_SIZE];
            hsize_t *sp;
            char *new_str;

            sp = (int *)attr_node->value;

            j = 0;
            n = 0;
            for(m = 0; m < attr_node->ndims; m++)
            {
                for(i=0; i<attr_node->dims[m]; i++)
                {
                    n += H5Rget_name(attr_id, H5R_OBJECT, sp + j, str, HDF5_BUF_SIZE);
                    j++;
                    n++;
                }
            }

            new_str = malloc(n*sizeof(char));
            j = 0;
            for(m = 0; m < attr_node->ndims; m++)
            {
                for(i=0; i<attr_node->dims[m]; i++)
                {
                    H5Rget_name(attr_id, H5R_OBJECT, sp + j, str, HDF5_BUF_SIZE);
                    if(j)
                        strcat(new_str, str);
                    else
                        strcpy(new_str, str);
                    strcat(new_str, ",");
                    j++;
                }
            }
            new_str[n-1] = '\0';
            free(attr_node->value);
            attr_node->value = new_str;

            attr_node->nbytes = n;
            strcpy(attr_node->type_name, "string");
        }

        H5Tclose(p_type);
    }

    H5Sclose(space);
    H5Tclose(type);

    cur_attr_list->next = *attr_list;
    *attr_list = cur_attr_list;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5check_attr, at file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif

    return SUCCEED;
}


/*
 *************************************************************************
 * Function:    _find_parent_group_name(char *name)
 *
 * Purpose:     find parent group name, based on (self)name.
 *
 * Return:      name of the parent group.
 *
 * Programmer:  Wei Huang
 *              August 24, 2009
 *
 *************************************************************************
 */

char *_find_parent_group_name(char *name)
{
    char *parent_group_name = NULL;
    string_queue_t *sq;
    string_list_t *sl;
    char fullname[HDF5_NAME_LEN];

    if(name[0] == '/')
    {
        strcpy(fullname, name);
    }
    else
    {
        strcpy(fullname, "/");
        strcat(fullname, name);
    }

    parent_group_name = calloc(strlen(name) + 2, sizeof(char));
    sq = _split_string2queue(fullname, "/");
    parent_group_name[0] = '\0';

    if(sq->ns > 1)
    {
        int i;

        sl = sq->head;
        for(i = 0; i < sq->ns - 2; i++)
        {
            strcat(parent_group_name, "/");
            strcat(parent_group_name, sl->str);
            sl = sl->next;
        }
    }

  /*
   *fprintf(stdout, "\tname    : <%s>\n", name);
   *fprintf(stdout, "\tparent_group_name: <%s>\n\n", parent_group_name);
   */

    _free_string_queue(sq);
    return parent_group_name;
}


/*
 *************************************************************************
 * Function:    _get_group_name(char *name)
 *
 * Purpose:     get group name, based on (self)name.
 *
 * Return:      name of the group.
 *
 * Programmer:  Wei Huang
 *              March 12, 2010
 *
 *************************************************************************
 */

char *_get_group_name(char *name)
{
    char *tmp_str;
    char *group_name;
    int len;

    group_name = calloc(strlen(name) + 2, sizeof(char));
    strcpy(group_name, name);

    tmp_str = strrchr(group_name, '/');
    len = strlen(group_name) - strlen(tmp_str);
    group_name[len] = '\0';

    return group_name;
}


/*
 *************************************************************************
 * Function:    _get_short_name(char *name)
 *
 * Purpose:     get short name from (long) name.
 *
 * Return:      short name (wihtout "/").
 *
 * Programmer:  Wei Huang
 *              August 24, 2009
 *
 *************************************************************************
 */

char *_get_short_name(char *name)
{
    char *tmp_str;
    char *short_name;

    tmp_str = strrchr(name, '/');
    short_name = (tmp_str + 1);

    return short_name;
}


/*
 *************************************************************************
 * Function:    _HDF5Dataset_is_new(char *name, NclHDF5group_node_t *group_node)
 *
 * Purpose:     Check if dataset is new
 *
 * Return:      1, dataset is new
 *              0, dataset is not new.
 *
 * Programmer:  Wei Huang
 *              August 24, 2009
 *
 *************************************************************************
 */

int _HDF5Dataset_is_new(char *name, NclHDF5group_node_t *group_node)
{
    NclHDF5dataset_node_t *dataset_node;
    NclHDF5dataset_list_t *dataset_list;

    int dataset_is_new = 1;

  /*
   *fprintf(stdout, "\n\n\tchecking dataset <%s>, at file: %s, line: %d\n\n", dataset_node->name, __FILE__, __LINE__);
   *fprintf(stdout, "\tfrom group <%s>, at file: %s, line: %d\n\n", group_node->name, __FILE__, __LINE__);
   */

    dataset_list = group_node->dataset_list;
    while(dataset_list)
    {
        dataset_node = dataset_list->dataset_node;
      /*
       *fprintf(stdout, "\n\n\tchecking dataset <%s>, at file: %s, line: %d\n\n", dataset_node->name, __FILE__, __LINE__);
       */
        if(0 == strcmp(name, dataset_node->name))
        {
          /*
           *fprintf(stdout, "\n\n\tdataset <%s> is already in, at file: %s, line: %d\n\n", name, __FILE__, __LINE__);
           */
            return 0;
        }
        dataset_list = dataset_list->next;
    }

    return dataset_is_new;
}



/*
 *************************************************************************
 * Function:    _HDF5Group_is_new(char *name, NclHDF5group_node_t *group_node)
 *
 * Purpose:     Search information about an object
 *
 * Return:      1, group is new
 *              0, group is not new.
 *
 * Programmer:  Wei Huang
 *              August 24, 2009
 *
 *************************************************************************
 */

int _HDF5Group_is_new(char *name, NclHDF5group_node_t *group_node)
{
    NclHDF5group_node_t *cur_node;
    NclHDF5group_list_t *cur_list;

    int group_is_new = 1;

    cur_list = group_node->group_list;
    while(group_is_new && cur_list)
    {
         cur_node = cur_list->group_node;
       /*
        *fprintf(stdout, "checking group <%s>, at file: %s, line: %d\n\n", cur_node->name, __FILE__, __LINE__);
        */
         if(0 == strcmp(name, cur_node->name))
         {
           /*
            *fprintf(stdout, "group <%s> is already in, at file: %s, line: %d\n\n", name, __FILE__, __LINE__);
            */
             return 0;
         }
         group_is_new = _HDF5Group_is_new(name, cur_list->group_node);
         cur_list = cur_list->next;
    }

    return group_is_new;
}


/*
 *************************************************************************
 * Function:    _find_HDF5Group(char *name, NclHDF5group_node_t *group_node)
 *
 * Purpose:     find group with name
 *
 * Return:      NclHDF5group_node_t *group_node, with name.
 *
 * Programmer:  Wei Huang
 *              August 24, 2009
 *
 *************************************************************************
 */

NclHDF5group_node_t *_find_HDF5Group(char *name, NclHDF5group_node_t *group_node)
{
    NclHDF5group_node_t *the_node = NULL;
    NclHDF5group_node_t *cur_node = NULL;
    NclHDF5group_list_t *cur_list;
    int group_is_new = 1;

    if(strlen(name) < 1)
        return group_node;

    cur_list = group_node->group_list;
    while(group_is_new && cur_list)
    {
         cur_node = cur_list->group_node;
       /*
        *fprintf(stdout, "checking group <%s>, at file: %s, line: %d\n\n", cur_node->name, __FILE__, __LINE__);
        */
         if(0 == strcmp(name, cur_node->name))
         {
           /*
            *fprintf(stdout, "group <%s> is already in, at file: %s, line: %d\n\n", name, __FILE__, __LINE__);
            */
             return cur_node;
         }
         the_node = _find_HDF5Group(name, cur_list->group_node);
         if(the_node != NULL)
             return the_node;
         cur_list = cur_list->next;
    }

    return the_node;
}


/*
 *************************************************************************
 * Function:	_NclHDF5search_obj
 *
 * Purpose:	Search information about an object
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 *              July 1, 2009
 *
 *************************************************************************
 */

herr_t _NclHDF5search_obj(char *name, H5O_info_t *oinfo,
                          void *_NclHDF5group, char *already_seen)
{
    NclHDF5attr_list_t  *curAttrList = NULL;
    NclHDF5group_node_t *group_node = (NclHDF5group_node_t *) _NclHDF5group;

    H5O_type_t obj_type = oinfo->type;          /* Type of the object */
    hid_t obj_id = -1;                          /* ID of object opened */
    hid_t id = group_node->id;

    char comment[HDF5_BUF_SIZE];
    char parent_group_name[HDF5_NAME_LEN];
    char short_name[HDF5_NAME_LEN];

    int it_is_root = 0;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5search_obj, int file: %s, line: %d\n", __FILE__, __LINE__);
    fprintf(stdout, "\tname = <%s>, id: %d\n", name, id);
#endif

    if(already_seen)
    {
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "ALREADY-SEEN in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tname: <%s>\n", name);
        fprintf(stdout, "ALREADY-SEEN in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "Leaving _NclHDF5search_obj, at file: %s, line: %d\n\n", __FILE__, __LINE__);
        return SUCCEED;
    }

    /* Open the object.  Not all objects can be opened.  If this is the case
     * then return right away.
     */
    obj_id = H5Oopen(id, name, H5P_DEFAULT);

    if((obj_type >= 0) && (obj_id < 0))
    {
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tname: <%s>\n", name);
        fprintf(stdout, "\tid: %d\n", id);
        fprintf(stdout, "\tobj_type: %d\n", obj_type);
        fprintf(stdout, "\tobj_id: %d\n", obj_id);
        fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "**************************************************************\n");
        fprintf(stdout, "\n\n\n");
        fprintf(stdout, "Leaving _NclHDF5search_obj, at file: %s, line: %d\n\n", __FILE__, __LINE__);
        return FAILED;
    }

    if(strcmp(name, "/"))
    {
        char *pgn = _find_parent_group_name(name);
        char *sn = _get_short_name(name);
        strcpy(parent_group_name, pgn);
        free(pgn);
        it_is_root = 0;
        strcpy(short_name, sn);
      /*
        fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
       */
    }
    else
    {
        it_is_root = 1;
        parent_group_name[0] = '\0';
        short_name[0] = '\0';
      /*
        fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
        fprintf(stdout, "\tit is the root\n");
       */
    }
   /*
    fprintf(stdout, "\tparent_group_name: <%s>\n", parent_group_name);
    fprintf(stdout, "\tshort_name: <%s>\n", short_name);
   */

    /* Check object information */
    switch (obj_type)
    {
        case H5O_TYPE_GROUP:
            H5Aiterate2(obj_id, H5_INDEX_NAME, H5_ITER_INC, NULL, _NclHDF5check_attr, &curAttrList);

            if(it_is_root)
            {
                group_node->type = obj_type;
                strcpy(group_node->name, "");
                strcpy(group_node->type_name, "root");

                group_node->attr_list = curAttrList;
                if(curAttrList)
                    group_node->num_attrs = curAttrList->attr_node->counter;
            }
            else
            {
                NclHDF5group_list_t *new_list;
                NclHDF5group_node_t *parent_node = _find_HDF5Group(parent_group_name, group_node);
                int new_group = 1;

                if(parent_node)
                {
                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\tfind parent group: <%s>\n", parent_node->name);
                   */
                    new_group = _HDF5Group_is_new(name, parent_node);
                }   
                else
                {
                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\tcan not find parent group for: <%s>\n", name);
                   *fprintf(stdout, "\tstrlen(parent_group_name) = %d\n", strlen(parent_group_name));
                   */
                    new_group = 1;
                    if(strlen(parent_group_name))
                    {
                      /*
                       */
                        fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                        fprintf(stdout, "\tcan not find parent group for: <%s>\n", name);
                        fprintf(stdout, "\tstrlen(parent_group_name) = %d\n", strlen(parent_group_name));
                        exit(-1);
                    }
                    else
                    {
                        parent_node = group_node;
                        new_group = _HDF5Group_is_new(name, parent_node);
                    }
                }

                if(new_group)
                {
                    NclHDF5group_node_t *new_node = _NclHDF5allocate_group(obj_id, NULL, name, obj_type);

                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\nnew group: <%s>\n", name);
                   */

                    new_list = calloc(1, sizeof(NclHDF5group_list_t));
                    if(new_list == NULL)
                    {
                        fprintf(stdout, "Failed to allocated memory for new_list.\n");
                        fprintf(stdout, "file: %s, line: %d\n", __FILE__, __LINE__);
                        return FAILED;
                    }

                    new_node->attr_list = curAttrList;

                    if(curAttrList)
                        new_node->num_attrs = curAttrList->attr_node->counter;

                    new_list->group_node = new_node;

                    new_list->next = parent_node->group_list;
                    parent_node->group_list = new_list;

                    parent_node->num_groups ++;
                }
                else
                {
                    NclHDF5group_node_t *new_node = _NclHDF5allocate_group(obj_id, NULL, name, obj_type);
                    NclHDF5group_node_t *self_node = _find_HDF5Group(name, group_node);

                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\nold group: <%s>\n", name);
                   */

                    if(self_node)
                    {
                      /*
                       *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                       *fprintf(stdout, "\tnew_node->name: <%s>\n", new_node->name);
                       *fprintf(stdout, "\tself_node->name: <%s>\n", self_node->name);
                       */

                        new_node->attr_list = curAttrList;

                        if(curAttrList)
                            new_node->num_attrs = curAttrList->attr_node->counter;

                        if(new_node->num_attrs != self_node->num_attrs)
                        {
                            fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                            fprintf(stdout, "\tnew_node->num_attrs: <%s>\n", new_node->num_attrs);
                            fprintf(stdout, "\tself_node->num_attrs: <%s>\n", new_node->num_attrs);
                            fprintf(stdout, "\tit is not new group, but number of attributes is different.\n");
                            exit(-1);
                        }
                        else
                        {
                            _NclHDF5free_group(new_node);
                        }
                    }
                    else
                    {
                        fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                        fprintf(stdout, "\tnew_node->name: <%s>\n", new_node->name);
                        fprintf(stdout, "\tit is not new group, but can not find the group.\n");
                        exit(-1);
                    }
                }
            }
            break;
        case H5O_TYPE_DATASET:
            {
                NclHDF5dataset_list_t *curHDF5dataset_list;
                NclHDF5dataset_node_t *dataset_node;
                NclHDF5group_node_t *parent_node = _find_HDF5Group(parent_group_name, group_node);
                int new_dataset = 1;
           
                if(parent_node)
                {
                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\tfind parent group: <%s>\n", parent_node->name);
                   *fprintf(stdout, "\tfor data set: <%s>\n", name);
                   */
                    new_dataset = _HDF5Dataset_is_new(name, parent_node);
                }   
                else
                {
                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\tcan not find parent group for dataset: <%s>\n", name);
                   *fprintf(stdout, "\tstrlen(parent_group_name) = %d\n", strlen(parent_group_name));
                   */
                    new_dataset = 1;
                    if(strlen(parent_group_name))
                        exit(-1);
                    else
                    {
                        parent_node = group_node;
                        new_dataset = _HDF5Dataset_is_new(name, parent_node);
                    }
                }

                if(new_dataset)
                {
                  /*
                   *fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                   *fprintf(stdout, "\nnew dataset: <%s>\n", name);
                   */

                    curHDF5dataset_list = calloc(1, sizeof(NclHDF5dataset_list_t));
                    if(!curHDF5dataset_list)
                    {
                        fprintf(stdout, "Failed to allocated memory for curHDF5dataset_list. in file: %s, line: %d\n",
                                __FILE__, __LINE__);
                        return FAILED;
                    }

                    dataset_node = calloc(1, sizeof(NclHDF5dataset_node_t));
                    if(!dataset_node)
                    {
                        fprintf(stdout, "Failed to allocated memory for dataset_node. in file: %s, line: %d\n",
                                __FILE__, __LINE__);
                        return FAILED;
                    }

                    curHDF5dataset_list->dataset_node = dataset_node;
                    curHDF5dataset_list->next = parent_node->dataset_list;
                    parent_node->dataset_list = curHDF5dataset_list;

                    strcpy(dataset_node->name, name);

                    strcpy(dataset_node->short_name, short_name);
                    strcpy(dataset_node->group_name, parent_group_name);

                    dataset_node->ndims = 0;
                    dataset_node->nchunkdims = 0;
                    dataset_node->deflate_pass = 1;
 
                    _NclHDF5dataset_info(obj_id, name, dataset_node);

                    H5Aiterate2(obj_id, H5_INDEX_NAME, H5_ITER_INC, NULL, _NclHDF5check_attr, &curAttrList);
                    dataset_node->attr_list = curAttrList;
                    if(curAttrList)
                        dataset_node->num_attrs = curAttrList->attr_node->counter;
                }
                else
                {
                    fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
                    fprintf(stdout, "\nold dataset: <%s>\n", name);
                }
            }
            break;
        case H5O_TYPE_NAMED_DATATYPE:
            fprintf(stdout, "\nin file: %s, line: %d\n", __FILE__, __LINE__);
            fprintf(stdout, "\tH5O_TYPE_NAMED_DATATYPE\n");
            fprintf(stdout, "\ttype obj_id   = %d\n", obj_id);
            fprintf(stdout, "\ttype obj_type = %d\n", obj_type);
#if 0
            strcpy(NclHDF5group_list->group_node->type_name, "Type");
            H5Aiterate2(obj_id, H5_INDEX_NAME, H5_ITER_INC, NULL, _NclHDF5check_attr, &curAttrList);
#endif
            break;
        default:
            strcpy(group_node->type_name, "unknown");
            group_node->type = H5O_TYPE_UNKNOWN;

            fprintf(stdout, "NclHDF5group_list->type: %d, NclHDF5group_list->type_name: <%s>\n",
                    group_node->type, group_node->type_name);
            fprintf(stdout, "Unknown obj_type in _NclHDF5search_obj. return FAILED.\n");
            H5Oclose(obj_id);
            return FAILED;
    }

#if 0
    /* Object comment */
    comment[0] = '\0';
    H5Oget_comment(obj_id, comment, sizeof(comment));
    strcpy(comment + sizeof(comment) - 4, "...");
    if(comment[0])
    {
        printf("    %-10s %s\"", "Comment:", comment);
        puts("\"");
    }
#endif

    /* Close the object. */
    H5Oclose(obj_id);

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5search_obj, at file: %s, line: %d\n\n", __FILE__, __LINE__);
#endif
    return SUCCEED;
}


/*
 *************************************************************************
 * Function:	_NclHDF5search_link
 *
 * Purpose:	Search information about an link
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 *              July 1, 2009
 *
 *************************************************************************
 */

herr_t _NclHDF5search_link(char *name, H5O_info_t *oinfo, void *_NclHDF5Rec)
{
    NclHDF5group_list_t *NclHDF5group_list = (NclHDF5group_list_t*) _NclHDF5Rec;
    char *buf;

#if DEBUG_NCL_HDF5
#endif
    fprintf(stdout, "\nEntering _NclHDF5search_link, name = <%s>, at file: %s, line: %d\n", name, __FILE__, __LINE__);

#if 0
    NclHDF5FileRec_t *NclHDF5FileRec = (NclHDF5FileRec_t*)_NclHDF5FileRec;

    switch(linfo->type)
    {
        case H5L_TYPE_SOFT:
            if((buf = malloc(linfo->u.val_size)) == NULL)
                return SUCCEED;

            if(H5Lget_val(NclHDF5FileRec->fid, name, buf, linfo->u.val_size, H5P_DEFAULT) < 0) {
                free(buf);
                return SUCCEED;
            } /* end if */

            fputs("Soft Link {", stderr);
            fputs(buf, stderr);
            free(buf);
            fputs("}\n", stderr);
            break;

        case H5L_TYPE_EXTERNAL:
            {
            const char *filename;
            const char *path;

            if((buf = malloc(linfo->u.val_size)) == NULL)
                return SUCCEED;
            if(H5Lget_val(NclHDF5FileRec->fid, name, buf, linfo->u.val_size, H5P_DEFAULT) < 0)
            {
                free(buf);
                return SUCCEED;
            }

            if(H5Lunpack_elink_val(buf, linfo->u.val_size, NULL, &filename, &path) < 0)
            {
                free(buf);
                return SUCCEED;
            } /* end if */

            fputs("External Link {", stderr);
            fputs(filename, stderr);
            fputc('/', stderr);
            if(*path != '/')
                fputc('/', stderr);
            fputs(path, stderr);
            fputc('}', stderr);

            /* Recurse through the external link */
            {
                fputc(' ', stderr);
            
                /* Check if we have already seen this elink */
                if(elink_trav_visited(NclHDF5FileRec->elink_list, filename, path))
                {
                    fputs("{Already Visited}\n", stderr);
                    free(buf);
                    return SUCCEED;
                }

                /* Add this link to the list of seen elinks */
                if(elink_trav_add(NclHDF5FileRec->elink_list, filename, path) < 0) {
                    free(buf);
                    return SUCCEED;
                }

                /* Adjust user data to specify that we are operating on the
                 * target of an external link */
                NclHDF5FileRec->ext_link = TRUE;

                /* Recurse through the external link */
                if(_NclHDF5visit_obj(NclHDF5FileRec->fid, name, NclHDF5FileRec) < 0)
                {
                    free(buf);
                    return SUCCEED;
                }
            }

            free(buf);
            }

            break;

        default:
            fputs("UD Link {cannot follow UD links}\n", stderr);
            break;
    }
#endif
#if DEBUG_NCL_HDF5
#endif
    fprintf(stdout, "Leaving _NclHDF5search_link, at file: %s, line: %d\n\n", __FILE__, __LINE__);
    return SUCCEED;
}


/*
 ***********************************************************************
 * Function:	_NclHDF5recursive_check
 *
 * Purpose:	Recursively check a group
 *
 * Return:	Success: SUCCEED
 *		Failure: FAILED
 *
 * Programmer:	Wei Huang
 * Created:	July 1, 2009
 *
 ***********************************************************************
 */

herr_t _NclHDF5recursive_check(hid_t fid, char *grp_name,
                               _NclHDF5search_obj_func_t _NclHDF5search_obj,
                               _NclHDF5search_link_func_t  _NclHDF5search_link,
                               NclHDF5group_node_t *HDF5roup)
{
    H5O_info_t  oinfo;          /* Object info for starting group */
    NclHDF5searcher_t searcher;

#if DEBUG_NCL_HDF5
    fprintf(stdout, "\nEntering _NclHDF5recursive_check, grp_name: <%s>, fid=%d, in file: %s, at line: %d\n",
            grp_name, fid, __FILE__, __LINE__);
#endif

    /* Init searcher structure */
    searcher._NclHDF5search_obj = _NclHDF5search_obj;
    searcher._NclHDF5search_link = _NclHDF5search_link;
    searcher.udata = (void *)HDF5roup;

    /* Get info for starting object */
    if(H5Oget_info_by_name(fid, grp_name, &oinfo, H5P_DEFAULT) < 0)
        return FAILED;

    /* Searching the object */
    if(_NclHDF5search_obj)
    {
        (_NclHDF5search_obj)(grp_name, &oinfo, searcher.udata, NULL);
    }

    /* if the object is a group */
    if(oinfo.type == H5O_TYPE_GROUP)
    {
        NclHDF5_addr_t        seen;     /* List of addresses seen */
        NclHDF5_ud_traverse_t tudata;   /* User data for iteration callback */

        /* Init addresses seen */
        seen.nused = seen.nalloc = 0;
        seen.objs = NULL;

        /* Check for multiple links to top group */
        if(oinfo.rc > 1)
            _NclHDF5_addr_add(&seen, oinfo.addr, grp_name);

        /* Set up user data structure */
        tudata.seen = &seen;
        tudata.searcher = &searcher;
        tudata.is_absolute = (*grp_name == '/');
        tudata.base_grp_name = grp_name;

        /* Visit all links in group, recursively */
        if(H5Lvisit_by_name(fid, grp_name, H5_INDEX_NAME, H5_ITER_INC, _NclHDF5search_by_name, &tudata, H5P_DEFAULT) < 0)
        {
            fprintf(stdout, "\n\n\n");
            fprintf(stdout, "**************************************************************\n");
            fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
            fprintf(stdout, "\tgrp_name: <%s>\n", grp_name);
            fprintf(stdout, "\tfid: %d\n", fid);
            fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
            fprintf(stdout, "**************************************************************\n");
            fprintf(stdout, "\n\n\n");
            fprintf(stdout, "Leaving _NclHDF5recursive_check, at file: %s, line: %d\n\n", __FILE__, __LINE__);

            return FAILED;
        }
#if 0	/* This seems to cause everything read twice */
        else
        {
            /* Iterate over links in group */
            if(H5Literate_by_name(fid, grp_name, H5_INDEX_NAME, H5_ITER_INC, NULL, _NclHDF5search_by_name, &tudata, H5P_DEFAULT) < 0)
            {
                fprintf(stdout, "\n\n\n");
                fprintf(stdout, "**************************************************************\n");
                fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
                fprintf(stdout, "\tgrp_name: <%s>\n", grp_name);
                fprintf(stdout, "\tfid: %d\n", fid);
                fprintf(stdout, "FAILED in file: %s, line: %d\n", __FILE__, __LINE__);
                fprintf(stdout, "**************************************************************\n");
                fprintf(stdout, "\n\n\n");
                fprintf(stdout, "Leaving _NclHDF5recursive_check, at file: %s, line: %d\n\n", __FILE__, __LINE__);
                return FAILED;
            }
        }
#endif

        /* Free visited addresses table */
        if(seen.objs)
        {
            free(seen.objs);
        }
    }

#if DEBUG_NCL_HDF5
    fprintf(stdout, "Leaving _NclHDF5recursive_check, in file: %s, at line: %d\n\n", __FILE__, __LINE__);
#endif

    return SUCCEED;
}

