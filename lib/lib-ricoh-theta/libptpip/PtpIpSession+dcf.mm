/*
 * Copyright Ricoh Company, Ltd. All rights reserved.
 */

#import "PtpIpSession+dcf.h"

@implementation PtpIpSession (dcf)

-(NSArray*)getDcfObjectHandles:(NSInteger)formatCode imageObjectHandle:(NSInteger)imageObjectHandle from:(NSDate*)date dcfParent:(PtpIpObjectInfo**)parentNode
{
    PtpIpObjectInfo* imageInfo = [self getObjectInfo:imageObjectHandle];
    if (!imageInfo) {
        LOG_WARN("object(%u) is not found.", imageObjectHandle);
        return nil;
    }
    if (imageInfo.parent_object==0) {
        LOG_DEBUG("object(%u) hsa no parent object.", imageObjectHandle);
        return nil;
    }
    PtpIpObjectInfo* parentInfo = [self getObjectInfo:imageInfo.parent_object];
    if (!parentInfo) {
        LOG_ERROR("object(%u) of parent-object(%u) is not found.", imageObjectHandle, imageInfo.parent_object);
        return nil;
    }
    if (parentNode) {
        *parentNode = parentInfo;
    }
    if ((parentInfo.object_format!=PTP::ObjectFormatCode::ASSOCIATION)
        or (parentInfo.association_type!=PTP::AssociationTypeCode::AncillaryData)) {
        LOG_DEBUG("object(%u) of parent-object(%u) is not a DCF-root.", imageObjectHandle, imageInfo.parent_object);
        return nil;
    }
    return [self getObjectHandles:formatCode parent:imageInfo.parent_object from:date];
}


-(NSArray*)getDcfObjectInfo:(NSInteger)imageObjectHandle dcfParent:(PtpIpObjectInfo**)parentNode
{
    NSArray* handles = [self getDcfObjectHandles:0 imageObjectHandle:imageObjectHandle from:nil dcfParent:parentNode];
    if (handles==nil) {
        return nil;
    }
    if (handles.count==0) {
        return [[NSArray alloc] init];
    }
    NSMutableArray* result = [[NSMutableArray alloc] init];
    for (NSNumber* h in handles) {
        PtpIpObjectInfo* info = [self getObjectInfo:[h integerValue]];
        if (!info) {
            LOG_ERROR("child-object(%u) is not found.", [h integerValue]);
        } else {
            [result addObject:info];
        }
    }
    return result;
}


-(BOOL)deleteDcfObject:(NSInteger)imageObjectHandle onDelete:(BOOL(^)(BOOL result, const PtpIpObjectInfo* info))block
{
    PtpIpObjectInfo* parentNode;
    NSArray* infos = [self getDcfObjectInfo:imageObjectHandle dcfParent:&parentNode];
    if ((!infos) or (infos.count==0)) {
        return [self deleteObject:imageObjectHandle];
    }
    LOG_DEBUG("target DCF-tree is:\n%s", [[infos description] UTF8String]);
    
    // Remove main image other than
    BOOL result = YES, r;
    PtpIpObjectInfo* imageInfo = nil;
    for (PtpIpObjectInfo* i in infos) {
        if (i.object_handle!=imageObjectHandle) {
            r = [self deleteObject:i.object_handle];
            if (!r) {
                LOG_ERROR("delete child-object(%u) failed.", i.object_handle);
            }
            if (block && !block(r, i)) return NO;
            result &= r;
        } else {
            imageInfo = i;
        }
    }
    // Remove main image
    r = [self deleteObject:imageObjectHandle];
    if (!r) {
        LOG_ERROR("delete child-object(%u) failed.", imageObjectHandle);
    }
    if (block && !block(r, imageInfo)) return NO;
    result &= r;
    
    // Remove DCF node
    if (parentNode) {
        r = [self deleteObject:parentNode.object_handle];
        if (!r) {
            LOG_ERROR("delete DCF-parent(%u) failed.", parentNode.object_handle);
        }
        if (block && !block(r, parentNode)) return NO;
        result &= r;
    }
    
    return result;
}

@end
